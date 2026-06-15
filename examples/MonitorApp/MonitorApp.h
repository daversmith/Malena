#pragma once
#ifndef MONITOR_APP_H
#define MONITOR_APP_H

#include <Malena/Engine/App/Application.h>
#include <Malena/Screen/ScreenReceiver.h>
#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Graphics/Text/Text.h>

// ── Monitor app (runs ON the Pi) ───────────────────────────────────────────────
// Display-only: pulls streams from the Pi's LOCAL MediaMTX (127.0.0.1) and shows
// them on the attached display. No capture/sender — this device only views.
// MediaMTX runs on the same Pi, so the source is loopback.

struct MonAdminManifest {
    static constexpr const char* name = "Admin";
    static constexpr const char* url  = "rtsp://127.0.0.1:8554/admin";
};

struct MonStudentManifest {
    static constexpr const char* name = "Student";
    static constexpr const char* url  = "rtsp://127.0.0.1:8554/student";
};

// Layout modes the monitor can show (the product spec: one stream, or split two).
// First build ships Split; AdminOnly/StudentOnly are wired in applyLayout() ready
// for a future input toggle.
enum class MonitorLayout { Split, AdminOnly, StudentOnly };

class MonitorApp : public ml::Application
{
    ml::ScreenReceiver<MonAdminManifest>   _admin;
    ml::ScreenReceiver<MonStudentManifest> _student;

    ml::Rectangle _header;
    ml::Text      _title;

    MonitorLayout _layout = MonitorLayout::Split;
public:
    MonitorApp();

    void onInit()  override;
    void onReady() override;

private:
    void applyLayout();
};

#endif // MONITOR_APP_H
