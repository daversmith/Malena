#include "ScreenShareDemo.h"

static constexpr float kWinW      = 1280.f;
static constexpr float kWinH      = 760.f;
static constexpr float kHeaderH   = 40.f;
static constexpr float kStatusH   = 30.f;
static constexpr float kStreamTop = kHeaderH + kStatusH;
static constexpr float kStreamH   = kWinH - kStreamTop;
static constexpr float kHalfW     = kWinW / 2.f;

ScreenShareDemo::ScreenShareDemo()
    : ml::Application(
        static_cast<unsigned int>(kWinW),
        static_cast<unsigned int>(kWinH),
        32,
        "Screen Share Demo")
{}

void ScreenShareDemo::onInit()
{
    _header.setSize({ kWinW, kHeaderH });
    _header.setPosition({ 0.f, 0.f });
    _header.setFillColor(sf::Color(30, 30, 30));

    _titleText.setString("Screen Share Demo — RTSP via GStreamer (admin | student)");
    _titleText.setCharacterSize(18);
    _titleText.setFillColor(sf::Color(220, 220, 220));
    _titleText.setPosition({ 12.f, 10.f });

    _leftStatus.setSize({ kHalfW, kStatusH });
    _leftStatus.setPosition({ 0.f, kHeaderH });
    _leftStatus.setFillColor(sf::Color(50, 50, 50));
    _leftStatus.setCharacterSize(13);

    _rightStatus.setSize({ kHalfW, kStatusH });
    _rightStatus.setPosition({ kHalfW, kHeaderH });
    _rightStatus.setFillColor(sf::Color(50, 50, 50));
    _rightStatus.setCharacterSize(13);

    _left.setSize({ kHalfW, kStreamH });
    _left.setPosition({ 0.f, kStreamTop });

    _right.setSize({ kHalfW, kStreamH });
    _right.setPosition({ kHalfW, kStreamTop });

    _hint.setSize({ kWinW, 50.f });
    _hint.setPosition({ 0.f, kStreamTop + (kStreamH - 50.f) / 2.f });
    _hint.setFillColor(sf::Color(0, 0, 0, 180));
    _hint.setString(
        "Publish H.264 to rtsp://<pi>:8554/admin or /student "
        "(ffmpeg / OBS / Larix Broadcaster)");
    _hint.setCharacterSize(16);

    addComponent(_header);
    addComponent(_titleText);
    addComponent(_leftStatus);
    addComponent(_rightStatus);
    addComponent(_left);
    addComponent(_right);
    addComponent(_hint);
}

void ScreenShareDemo::onReady()
{
    _left.start();
    _right.start();
    _share.start();   // begin capturing this desktop and publishing to /admin
    _left.onUpdate([this]() { updateStatus(); });
}

void ScreenShareDemo::updateStatus()
{
    auto applyStatus = [](ml::RectangleButton& btn, const char* label, bool live)
    {
        if (live) {
            btn.setFillColor(sf::Color(20, 120, 40));
            btn.setString(std::string(label) + "  ●  Live");
        } else {
            btn.setFillColor(sf::Color(50, 50, 50));
            btn.setString(std::string(label) + "  ○  Waiting for stream...");
        }
    };

    bool leftOk  = _left.isConnected();
    bool rightOk = _right.isConnected();

    applyStatus(_leftStatus,  "Admin",   leftOk);
    applyStatus(_rightStatus, "Student", rightOk);

    if (leftOk || rightOk)
        _hint.enableFlag(ml::Flag::HIDDEN);
    else
        _hint.disableFlag(ml::Flag::HIDDEN);
}
