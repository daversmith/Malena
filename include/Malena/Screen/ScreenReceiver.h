// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#pragma once
#ifndef MALENA_SCREEN_RECEIVER_H
#define MALENA_SCREEN_RECEIVER_H

#include <Malena/Core/malena_export.h>
#include <Malena/Graphics/Base/Graphic.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace ml {

/**
 * @brief Displays a live H.264 video stream pulled from an RTSP/RTP source.
 *
 * Unlike the AirPlay receiver (which reverse-engineers Apple's protocol and
 * depends on GPL FairPlay code), this widget is a clean GStreamer client:
 * it pulls an H.264 stream from an RTSP server (e.g. MediaMTX running on the
 * Pi) — fed by Larix Broadcaster, OBS, a desktop capture pipeline, or our own
 * future capture app — decodes it (hardware-accelerated where available), and
 * uploads each frame to an sf::Texture for compositing in the Malena UI.
 *
 * The GStreamer pipeline runs on its own streaming threads; the latest frame
 * is pulled and uploaded to the texture from the render loop (draw()), so no
 * extra synchronisation is needed.  If the source is not yet publishing, the
 * pipeline auto-retries with a short backoff and a black placeholder is shown.
 *
 * @see ScreenReceiver
 */
class MALENA_API ScreenReceiverBase : public Graphic<sf::RectangleShape>
{
public:
    /**
     * How the decoded frame is fit into the receiver's rect:
     *  - Stretch: fill the rect, distorting aspect (the default; "skew").
     *  - Fit:     uniform scale, fully visible + centered, letterboxed ("lightbox").
     *  - Fill:    uniform scale to cover the rect, centre-cropping the overflow.
     */
    enum class ScaleMode { Stretch, Fit, Fill };

    /**
     * @param name    Human-readable label for logs/status.
     * @param rtspUrl Source URL, e.g. "rtsp://127.0.0.1:8554/admin".
     */
    ScreenReceiverBase(const char* name, const char* rtspUrl);
    ~ScreenReceiverBase() override;

    ScreenReceiverBase(const ScreenReceiverBase&)            = delete;
    ScreenReceiverBase& operator=(const ScreenReceiverBase&) = delete;

    /** Start (or restart) the GStreamer pipeline. Non-blocking. */
    void start();

    /** Stop the pipeline and release resources. */
    void stop();

    /**
     * Point this receiver at a different source. If the pipeline is running it
     * restarts on the new URL (so a pane can be switched between sources — e.g.
     * admin desktop, tablet, or @c student-{id} — at runtime). No-op if unchanged.
     */
    void setUrl(const std::string& rtspUrl);

    /**
     * Freeze the displayed image: draw() keeps rendering the last decoded frame
     * but stops pulling new ones (pipeline stays warm so unfreezing is instant).
     * Lets the director hold a pane while talking over it.
     */
    void setFrozen(bool frozen);
    bool isFrozen() const;

    /** How to fit the frame into the rect (default Stretch). */
    void setScaleMode(ScaleMode mode);
    ScaleMode scaleMode() const;

    /**
     * Display a frame handed to us directly, instead of pulling one from a
     * stream. This is the push path used by native screen sharing: the sender
     * captures and encodes locally and the frame arrives over the app's own
     * control socket, so there is no RTSP source to connect to and no media
     * runtime involved.
     *
     * Accepts any still image format SFML can read (JPEG today, whatever the
     * codec becomes later). Marks the receiver connected, so existing status UI
     * keeps working, and honours setFrozen()/setScaleMode() exactly as the
     * streaming path does.
     *
     * Safe to mix with start()/stop(): a pushed frame simply replaces whatever
     * is currently displayed.
     */
    void pushFrame(const std::uint8_t* data, std::size_t size);

    /**
     * Play a LOCAL FILE instead of a live stream.
     *
     * WHY THIS IS HERE and not a separate player: everything below the source —
     * the appsink, the RGBA upload into an sf::Texture, the scale modes, the
     * freeze — is identical, and so is the hard constraint that made this class
     * awkward in the first place. decodebin and RGBA caps can auto-plug GL
     * elements which activate a second GL context and invalidate SFML's
     * textures; the fix is a forced software path, and it took a crash to find.
     * A second player would have to rediscover that.
     *
     * The classroom reason: streaming a video to thirty students means thirty
     * live copies of the same bytes, which no classroom Wi-Fi carries. Sending
     * the file once and playing it locally costs a fraction of the airtime and
     * looks better, so long as everyone starts together — see playAt().
     */
    void setFile(const std::string& absolutePath);

    /**
     * Start playback at a shared moment.
     *
     * @param startAtUnixMs  Wall-clock instant to begin, as Unix milliseconds.
     *                       In the past means "start now". The instructor's hub
     *                       picks one a couple of seconds out and sends it to
     *                       every student, so they begin together rather than
     *                       whenever each machine happened to get the message.
     * @param positionMs     Offset within the video to begin at, for resuming
     *                       or seeking mid-lesson.
     */
    void playAt(long long startAtUnixMs, long long positionMs = 0);

    /** Hold playback where it is. The frame stays on screen. */
    void pausePlayback();

    /** Current position within the file, in milliseconds (0 when not playing). */
    long long positionMs() const;

    /** True while a local file is loaded (whether or not it is playing). */
    bool isFilePlayback() const;

    /** True once decoded frames are arriving. */
    bool isConnected() const;

    const std::string& receiverName() const;
    const std::string& url() const;

    // sf::Drawable override — uploads the latest frame and renders it (or a
    // black placeholder when no stream is active).
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

// ---------------------------------------------------------------------------

/**
 * @brief Typed screen receiver — reads config from a manifest struct.
 *
 * @code
 * struct AdminManifest {
 *     static constexpr const char* name = "Admin";
 *     static constexpr const char* url  = "rtsp://127.0.0.1:8554/admin";
 * };
 * panel.add<ml::ScreenReceiver<AdminManifest>>();
 * @endcode
 */
template<typename Manifest>
class ScreenReceiver : public ScreenReceiverBase
{
public:
    ScreenReceiver()
        : ScreenReceiverBase(Manifest::name, Manifest::url)
    {}
};

} // namespace ml

#endif // MALENA_SCREEN_RECEIVER_H
