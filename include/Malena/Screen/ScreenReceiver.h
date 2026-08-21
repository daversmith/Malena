// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#pragma once
#ifndef MALENA_SCREEN_RECEIVER_H
#define MALENA_SCREEN_RECEIVER_H

#include <Malena/Core/malena_export.h>
#include <Malena/Graphics/Base/Graphic.h>
#include <SFML/Graphics/RectangleShape.hpp>
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
     * admin desktop ↔ tablet ↔ student-<id> — at runtime). No-op if unchanged.
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
