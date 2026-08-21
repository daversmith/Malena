#pragma once
#ifndef SCREENSHARE_DEMO_H
#define SCREENSHARE_DEMO_H

#include <Malena/Engine/App/Application.h>
#include <Malena/Screen/ScreenReceiver.h>
#include <Malena/Screen/ScreenSender.h>
#include <Malena/Graphics/Controls/RectangleButton.h>
#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Graphics/Text/Text.h>

// ── Stream manifests ──────────────────────────────────────────────────────────
// Each pane pulls an H.264 RTSP stream from MediaMTX (running locally for the
// POC). Publish to these paths from ffmpeg / OBS / Larix Broadcaster.

// All streams relay through the Pi hub (MediaMTX on the hotspot at .4.162:8554).
struct AdminManifest {
    static constexpr const char* name = "Admin";
    static constexpr const char* url  = "rtsp://192.168.4.162:8554/admin";
};

struct StudentManifest {
    static constexpr const char* name = "Student";
    static constexpr const char* url  = "rtsp://192.168.4.162:8554/student";
};

// This machine's desktop is captured and published to /admin on the Pi (the
// same path the left pane pulls), demonstrating the admin self-share path
// routed through the Pi relay.
struct AdminShareManifest {
    static constexpr const char* name = "Admin desktop";
    static constexpr const char* url  = "rtsp://192.168.4.162:8554/admin";
};

// ── App ───────────────────────────────────────────────────────────────────────

class ScreenShareDemo : public ml::Application
{
    ml::ScreenReceiver<AdminManifest>   _left;
    ml::ScreenReceiver<StudentManifest> _right;
    ml::ScreenSender<AdminShareManifest> _share;   // captures this desktop → /admin

    ml::Rectangle       _header;
    ml::Text            _titleText;
    ml::RectangleButton _leftStatus;
    ml::RectangleButton _rightStatus;
    ml::RectangleButton _hint;
public:
    ScreenShareDemo();

    void onInit()  override;
    void onReady() override;

private:
    void updateStatus();
};

#endif // SCREENSHARE_DEMO_H
