// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#include <Malena/Screen/ScreenReceiver.h>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/video/video.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <string>
#include <vector>

namespace ml {

namespace {
    // gst_init must run exactly once per process.
    void ensureGstInit()
    {
        static std::once_flag once;
        std::call_once(once, [] { gst_init(nullptr, nullptr); });
    }

    // Choose the H.264 decoder. We use software (avdec_h264) on every platform:
    //  - macOS: the hardware path (vtdec) auto-plugs GL surfaces that clobber
    //    SFML's GL context.
    //  - Raspberry Pi 4: the V4L2 hardware decoder (v4l2h264dec / bcm2835-codec)
    //    works but its buffer pool adds ~3s of fixed latency. For the light
    //    stream we send (720p/15fps) software decode keeps up in real time with
    //    sub-second latency (measured identical to the macOS receiver), so it's
    //    the better choice here. (Revisit HW decode only for heavy/many streams.)
    const char* chooseH264Decoder()
    {
        return "avdec_h264";
    }
}

// =============================================================================
// Impl
// =============================================================================

struct ScreenReceiverBase::Impl
{
    std::string  name;
    std::string  url;

    GstElement*  pipeline = nullptr;
    GstAppSink*  sink     = nullptr;

    mutable sf::Texture texture;
    bool         textureReady = false;
    std::atomic<bool> connected { false };

    using clock = std::chrono::steady_clock;
    clock::time_point lastFrame {};
    clock::time_point lastStart {};
    bool         wantRestart = false;
    bool         running     = false;
    bool         frozen      = false;   // hold the last frame; stop pulling new ones
    ScaleMode    scaleMode   = ScaleMode::Stretch;

    std::vector<uint8_t> rgba;   // reused scratch buffer

    std::string buildPipeline() const
    {
        // RTSP H.264 → CPU decode → CPU convert → RGBA → appsink.
        //
        // IMPORTANT (macOS): we deliberately force a 100% software path
        // (avdec_h264 + videoconvert) instead of decodebin. decodebin / the
        // RGBA caps can auto-plug GL elements (glupload/gldownload, vtdec GL
        // surfaces) which activate a *separate* GL context and invalidate
        // SFML's textures (crash in sf::Texture::bind). A pure-CPU path never
        // touches GL, so SFML's context stays intact. (Hardware decode on the
        // Pi will use a non-GL v4l2 path; revisit per-platform later.)
        const std::string dec = chooseH264Decoder();
        return
            "rtspsrc location=\"" + url + "\" latency=100 drop-on-latency=true ! "
            "rtph264depay ! h264parse ! " + dec + " ! "
            "videoconvert ! video/x-raw,format=RGBA ! "
            "appsink name=sink sync=false max-buffers=1 drop=true";
    }

    void startPipeline()
    {
        stopPipeline();

        GError* err = nullptr;
        const std::string desc = buildPipeline();
        pipeline = gst_parse_launch(desc.c_str(), &err);
        if (!pipeline) {
            std::fprintf(stderr, "[ScreenReceiver:%s] parse failed: %s\n",
                         name.c_str(), err ? err->message : "?");
            if (err) g_error_free(err);
            return;
        }
        if (err) g_error_free(err);

        if (GstElement* s = gst_bin_get_by_name(GST_BIN(pipeline), "sink"))
            sink = GST_APP_SINK(s);   // keeps the ref returned by get_by_name

        gst_element_set_state(pipeline, GST_STATE_PLAYING);

        lastStart   = clock::now();
        lastFrame   = lastStart;
        wantRestart = false;
        std::fprintf(stderr, "[ScreenReceiver:%s] pipeline PLAYING → %s\n",
                     name.c_str(), url.c_str());
    }

    void stopPipeline()
    {
        if (sink)     { gst_object_unref(sink);     sink     = nullptr; }
        if (pipeline) {
            gst_element_set_state(pipeline, GST_STATE_NULL);
            gst_object_unref(pipeline);
            pipeline = nullptr;
        }
    }

    // Drain ERROR/EOS messages so we can trigger a reconnect.
    void pollBus()
    {
        if (!pipeline) return;
        GstBus* bus = gst_element_get_bus(pipeline);
        GstMessage* msg;
        while ((msg = gst_bus_pop_filtered(
                    bus, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS))))
        {
            if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
                GError* e = nullptr; gchar* dbg = nullptr;
                gst_message_parse_error(msg, &e, &dbg);
                std::fprintf(stderr, "[ScreenReceiver:%s] gst error: %s\n",
                             name.c_str(), e ? e->message : "?");
                if (e) g_error_free(e);
                g_free(dbg);
            }
            wantRestart = true;
            gst_message_unref(msg);
        }
        gst_object_unref(bus);
    }

    // Pull the most recent decoded frame (non-blocking) into the texture.
    bool pullFrame()
    {
        if (!sink) return false;
        GstSample* sample = gst_app_sink_try_pull_sample(sink, 0);
        if (!sample) return false;

        bool ok = false;
        GstCaps*   caps = gst_sample_get_caps(sample);
        GstBuffer* buf  = gst_sample_get_buffer(sample);
        GstVideoInfo info;
        if (caps && buf && gst_video_info_from_caps(&info, caps)) {
            GstVideoFrame vf;
            if (gst_video_frame_map(&vf, &info, buf, GST_MAP_READ)) {
                const unsigned w = GST_VIDEO_FRAME_WIDTH(&vf);
                const unsigned h = GST_VIDEO_FRAME_HEIGHT(&vf);
                const auto* src  = static_cast<const uint8_t*>(
                                       GST_VIDEO_FRAME_PLANE_DATA(&vf, 0));
                const int stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vf, 0);

                // Tightly pack rows (GStreamer stride may be padded).
                rgba.resize(static_cast<size_t>(w) * h * 4);
                for (unsigned y = 0; y < h; ++y)
                    std::memcpy(rgba.data() + static_cast<size_t>(y) * w * 4,
                                src + static_cast<size_t>(y) * stride,
                                static_cast<size_t>(w) * 4);
                gst_video_frame_unmap(&vf);

                if (!textureReady ||
                    texture.getSize().x != w || texture.getSize().y != h) {
                    (void)texture.resize({ w, h });
                }
                texture.update(rgba.data());
                textureReady = true;
                ok = true;
            }
        }
        gst_sample_unref(sample);
        return ok;
    }
};

// =============================================================================
// ScreenReceiverBase
// =============================================================================

ScreenReceiverBase::ScreenReceiverBase(const char* name, const char* rtspUrl)
    : _impl(std::make_unique<Impl>())
{
    ensureGstInit();
    _impl->name = name ? name : "Screen";
    _impl->url  = rtspUrl ? rtspUrl : "";
    this->setFillColor(sf::Color::Black);
    this->setSize({ 320.f, 240.f });
}

ScreenReceiverBase::~ScreenReceiverBase()
{
    _impl->stopPipeline();
}

void ScreenReceiverBase::start()
{
    _impl->running = true;
    _impl->startPipeline();
}

void ScreenReceiverBase::stop()
{
    _impl->running = false;
    _impl->stopPipeline();
    _impl->connected.store(false);
}

void ScreenReceiverBase::setUrl(const std::string& rtspUrl)
{
    if (rtspUrl == _impl->url) return;
    _impl->url = rtspUrl;
    _impl->connected.store(false);
    _impl->textureReady = false;          // drop the stale source's last frame
    if (_impl->running) _impl->startPipeline();   // re-aim at the new source
}

void ScreenReceiverBase::setFrozen(bool frozen) { _impl->frozen = frozen; }
bool ScreenReceiverBase::isFrozen() const { return _impl->frozen; }

void ScreenReceiverBase::setScaleMode(ScaleMode mode) { _impl->scaleMode = mode; }
ScreenReceiverBase::ScaleMode ScreenReceiverBase::scaleMode() const { return _impl->scaleMode; }

bool ScreenReceiverBase::isConnected() const { return _impl->connected.load(); }
const std::string& ScreenReceiverBase::receiverName() const { return _impl->name; }
const std::string& ScreenReceiverBase::url() const { return _impl->url; }

void ScreenReceiverBase::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    Impl* d = _impl.get();   // pointee is mutable even from a const method

    // When frozen, keep the pipeline warm but stop updating the texture — draw()
    // below just re-renders the last decoded frame.
    if (d->running && !d->frozen) {
        d->pollBus();
        const auto now = Impl::clock::now();

        if (d->pullFrame()) {
            d->lastFrame = now;
            d->connected.store(true);
        } else {
            using namespace std::chrono;
            const bool stalled =
                duration_cast<seconds>(now - d->lastFrame).count() >= 3;
            const bool cooled =
                duration_cast<milliseconds>(now - d->lastStart).count() >= 1000;
            if ((d->wantRestart || stalled) && cooled) {
                d->connected.store(false);
                d->startPipeline();   // auto-reconnect (source not up yet, etc.)
            }
        }
    }

    if (d->textureReady) {
        const sf::Vector2f pos = this->getPosition();
        const sf::Vector2f sz  = this->getSize();
        const sf::Vector2u tsz = d->texture.getSize();
        sf::Sprite s(d->texture);

        if (tsz.x > 0 && tsz.y > 0) {
            const float tw = static_cast<float>(tsz.x), th = static_cast<float>(tsz.y);
            switch (d->scaleMode) {
            case ScaleMode::Stretch:                       // fill, distort aspect
                s.setPosition(pos);
                s.setScale({ sz.x / tw, sz.y / th });
                break;
            case ScaleMode::Fit: {                         // letterbox, centered
                const float k = std::min(sz.x / tw, sz.y / th);
                s.setScale({ k, k });
                s.setPosition({ pos.x + (sz.x - tw * k) * 0.5f,
                                pos.y + (sz.y - th * k) * 0.5f });
                break;
            }
            case ScaleMode::Fill: {                        // cover, centre-crop overflow
                const float k = std::max(sz.x / tw, sz.y / th);
                const float visW = std::min(tw, sz.x / k);
                const float visH = std::min(th, sz.y / k);
                s.setTextureRect(sf::IntRect(
                    { static_cast<int>((tw - visW) * 0.5f),
                      static_cast<int>((th - visH) * 0.5f) },
                    { static_cast<int>(visW), static_cast<int>(visH) }));
                s.setScale({ k, k });
                s.setPosition(pos);
                break;
            }
            }
        } else {
            s.setPosition(pos);
        }
        target.draw(s, states);
    } else {
        sf::RectangleShape bg(this->getSize());
        bg.setPosition(this->getPosition());
        bg.setFillColor(sf::Color(10, 10, 10));
        bg.setOutlineColor(sf::Color(60, 60, 60));
        bg.setOutlineThickness(1.f);
        target.draw(bg, states);
    }
}

} // namespace ml
