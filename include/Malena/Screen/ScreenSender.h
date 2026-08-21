// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#pragma once
#ifndef MALENA_SCREEN_SENDER_H
#define MALENA_SCREEN_SENDER_H

#include <Malena/Core/malena_export.h>
#include <memory>
#include <string>

namespace ml {

/**
 * @brief Captures this machine's desktop and publishes it as an H.264 RTSP
 *        stream (the sending counterpart to ScreenReceiver).
 *
 * Headless (no UI): it captures the screen with a platform-native GStreamer
 * source, encodes with a platform-native **non-GPL** H.264 encoder, and pushes
 * the stream to an RTSP server (e.g. MediaMTX on the Pi) via rtspclientsink.
 * A Malena admin app constructs one of these to share its own desktop; the
 * stream is then displayed by any ScreenReceiver pulling the same path.
 *
 *   capture → videoconvert → H.264 encode → h264parse → rtspclientsink(URL)
 *
 * Encoders are chosen to keep the product distributable (no GPL/x264):
 *   - macOS   : VideoToolbox (vtenc_h264)          + avfvideosrc capture-screen
 *   - Windows : Media Foundation (mfh264enc)        + d3d11screencapturesrc
 *   - Linux/Pi: V4L2 stateful HW encoder (v4l2h264enc) + ximagesrc
 *
 * The pipeline runs on its own GStreamer threads with a background watcher that
 * auto-reconnects (with backoff) if the RTSP server is not yet up or drops.
 *
 * @see ScreenReceiver
 */
class MALENA_API ScreenSenderBase
{
public:
    /**
     * @param name    Human-readable label for logs/status.
     * @param rtspUrl Publish target, e.g. "rtsp://192.168.4.1:8554/admin".
     */
    ScreenSenderBase(const char* name, const char* rtspUrl);
    virtual ~ScreenSenderBase();

    ScreenSenderBase(const ScreenSenderBase&)            = delete;
    ScreenSenderBase& operator=(const ScreenSenderBase&) = delete;

    /** Start (or restart) capturing + publishing. Non-blocking. */
    void start();

    /** Stop publishing and release the pipeline. */
    void stop();

    /**
     * Change the publish destination. Restarts the pipeline if running, so the
     * caller can derive the URL at runtime (e.g. from a configured media host)
     * rather than fixing it at construction. No-op if unchanged.
     */
    void setUrl(const std::string& rtspUrl);

    /**
     * Choose which display (monitor) to capture. macOS: avfvideosrc device-index;
     * Windows: d3d11screencapturesrc monitor-index; Linux currently ignores it
     * (whole primary screen). Restarts if running. Default 0 (primary display).
     * Also selects whole-display capture mode (the default).
     */
    void setCaptureIndex(int index);

    /**
     * Capture a single window instead of a whole display (macOS only). Streams via
     * an external helper (set with setWindowHelperPath) that uses ScreenCaptureKit,
     * since the built-in GStreamer screen sources can only grab a full display.
     * If no helper is configured, falls back to whole-display capture. Restarts if
     * running.
     *
     * @param windowId Native window id (macOS CGWindowID / SCWindow.windowID).
     */
    void setCaptureWindow(unsigned int windowId);

    /**
     * Path to the per-window capture helper executable (e.g. LockInWindowCapture).
     * Spawned with `--window-id <id> --url <url>` when in window-capture mode.
     */
    void setWindowHelperPath(const std::string& path);

    /** True while the pipeline is live (PLAYING with no recent error). */
    bool isPublishing() const;

    const std::string& senderName() const;
    const std::string& url() const;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

// ---------------------------------------------------------------------------

/**
 * @brief Typed screen sender — reads config from a manifest struct.
 *
 * @code
 * struct AdminShareManifest {
 *     static constexpr const char* name = "Admin desktop";
 *     static constexpr const char* url  = "rtsp://192.168.4.1:8554/admin";
 * };
 * ml::ScreenSender<AdminShareManifest> share;
 * share.start();
 * @endcode
 */
template<typename Manifest>
class ScreenSender : public ScreenSenderBase
{
public:
    ScreenSender()
        : ScreenSenderBase(Manifest::name, Manifest::url)
    {}
};

} // namespace ml

#endif // MALENA_SCREEN_SENDER_H
