// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#include <Malena/Screen/ScreenSender.h>

#include <gst/gst.h>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <mutex>
#include <string>
#include <thread>

namespace ml {

namespace {
    // gst_init must run exactly once per process (idempotent, but guard anyway).
    void ensureGstInit()
    {
        static std::once_flag once;
        std::call_once(once, [] { gst_init(nullptr, nullptr); });
    }
}

// =============================================================================
// Impl
// =============================================================================

struct ScreenSenderBase::Impl
{
    std::string  name;
    std::string  url;

    GstElement*  pipeline = nullptr;

    std::atomic<bool> publishing { false };
    std::atomic<bool> running    { false };
    std::thread       watcher;

    // Per-platform: desktop capture → convert → non-GPL H.264 encode → RTSP push.
    std::string buildPipeline() const
    {
        // rtspclientsink publishes (RTSP RECORD) to the given location.
        const std::string sink =
            " h264parse ! rtspclientsink location=\"" + url + "\" latency=0";

#if defined(__APPLE__)
        // VideoToolbox HW encoder (vtenc_h264) — Apple-provided, not GPL.
        // avfvideosrc capture-screen grabs the whole display.
        //
        // Downscale to 720p and cap to 15fps BEFORE encoding: sending a full
        // high-res Retina desktop at native fps overwhelms the receiver's
        // decoder, frames queue, and end-to-end latency grows to many seconds.
        // A light 720p/15fps stream is decoded in real time → low latency.
        // videoscale add-borders preserves aspect ratio (letterboxed).
        return
            "avfvideosrc capture-screen=true capture-screen-cursor=true ! "
            "video/x-raw ! videorate ! videoscale add-borders=true ! "
            "video/x-raw,width=1280,height=720,framerate=15/1 ! "
            "videoconvert ! "
            "vtenc_h264 realtime=true allow-frame-reordering=false "
            "max-keyframe-interval=15 bitrate=4000 !" + sink;

#elif defined(_WIN32)
        // Media Foundation HW encoder (mfh264enc) — shipped with Windows, not GPL.
        return
            "d3d11screencapturesrc ! videoconvert ! "
            "mfh264enc low-latency=true bitrate=6000 !" + sink;

#else
        // Linux / Raspberry Pi: V4L2 stateful HW encoder (v4l2h264enc) — not GPL.
        // ximagesrc for X11; Wayland targets should swap in pipewiresrc.
        return
            "ximagesrc use-damage=false ! videoconvert ! "
            "video/x-raw,format=I420 ! v4l2h264enc "
            "extra-controls=\"controls,video_bitrate=6000000\" !" + sink;
#endif
    }

    void startPipeline()
    {
        stopPipeline();

        GError* err = nullptr;
        const std::string desc = buildPipeline();
        pipeline = gst_parse_launch(desc.c_str(), &err);
        if (!pipeline) {
            std::fprintf(stderr, "[ScreenSender:%s] parse failed: %s\n",
                         name.c_str(), err ? err->message : "?");
            if (err) g_error_free(err);
            return;
        }
        if (err) g_error_free(err);

        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        publishing.store(true);
        std::fprintf(stderr, "[ScreenSender:%s] capturing → publishing %s\n",
                     name.c_str(), url.c_str());
    }

    void stopPipeline()
    {
        publishing.store(false);
        if (pipeline) {
            gst_element_set_state(pipeline, GST_STATE_NULL);
            gst_object_unref(pipeline);
            pipeline = nullptr;
        }
    }

    // Background watcher: handle ERROR/EOS and auto-reconnect with backoff.
    void watch()
    {
        using namespace std::chrono;
        while (running.load()) {
            if (!pipeline) { std::this_thread::sleep_for(milliseconds(200)); continue; }

            GstBus* bus = gst_element_get_bus(pipeline);
            GstMessage* msg = gst_bus_timed_pop_filtered(
                bus, 200 * GST_MSECOND,
                (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
            gst_object_unref(bus);

            if (!msg) continue;   // timed out — pipeline still healthy

            if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
                GError* e = nullptr; gchar* dbg = nullptr;
                gst_message_parse_error(msg, &e, &dbg);
                std::fprintf(stderr, "[ScreenSender:%s] gst error: %s\n",
                             name.c_str(), e ? e->message : "?");
                if (e) g_error_free(e);
                g_free(dbg);
            }
            gst_message_unref(msg);

            // Server not up yet / dropped — back off, then retry.
            publishing.store(false);
            if (running.load()) {
                std::this_thread::sleep_for(seconds(1));
                if (running.load()) startPipeline();
            }
        }
    }
};

// =============================================================================
// ScreenSenderBase
// =============================================================================

ScreenSenderBase::ScreenSenderBase(const char* name, const char* rtspUrl)
    : _impl(std::make_unique<Impl>())
{
    ensureGstInit();
    _impl->name = name ? name : "Share";
    _impl->url  = rtspUrl ? rtspUrl : "";
}

ScreenSenderBase::~ScreenSenderBase()
{
    stop();
}

void ScreenSenderBase::start()
{
    if (_impl->running.load()) return;
    _impl->running.store(true);
    _impl->startPipeline();
    _impl->watcher = std::thread([d = _impl.get()] { d->watch(); });
}

void ScreenSenderBase::stop()
{
    _impl->running.store(false);
    if (_impl->watcher.joinable()) _impl->watcher.join();
    _impl->stopPipeline();
}

bool ScreenSenderBase::isPublishing() const { return _impl->publishing.load(); }
const std::string& ScreenSenderBase::senderName() const { return _impl->name; }
const std::string& ScreenSenderBase::url() const { return _impl->url; }

} // namespace ml
