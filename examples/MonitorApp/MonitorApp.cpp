#include "MonitorApp.h"

#include <Malena/Engine/Window/WindowManager.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/VideoMode.hpp>

static constexpr float kHeaderH = 36.f;

// Open at the actual desktop/VNC resolution so the window isn't clamped (a
// clamped window leaves SFML's view larger than the framebuffer → squished /
// garbled output). Falls back to 1280x720 if no display is detected.
static sf::Vector2u desktopSize()
{
    sf::Vector2u s = sf::VideoMode::getDesktopMode().size;
    if (s.x == 0 || s.y == 0) s = { 1280u, 720u };
    return s;
}

MonitorApp::MonitorApp()
    : ml::Application(desktopSize().x, desktopSize().y, 32, "TappedIn Monitor")
{}

void MonitorApp::onInit()
{
    _header.setFillColor(sf::Color(20, 20, 24));

    _title.setString("TappedIn Monitor — live (admin | student)");
    _title.setCharacterSize(16);
    _title.setFillColor(sf::Color(210, 210, 210));

    addComponent(_header);
    addComponent(_title);
    addComponent(_admin);
    addComponent(_student);

    applyLayout();
}

void MonitorApp::applyLayout()
{
    // Lay everything out from the LIVE window size, never hardcoded constants,
    // so the split stays even whatever size the window actually ends up.
    const sf::Vector2u ws = ml::WindowManager::getWindow().getSize();
    const float W    = static_cast<float>(ws.x);
    const float H    = static_cast<float>(ws.y);
    const float top  = kHeaderH;
    const float full = H - top;
    const float half = W / 2.f;

    _header.setSize({ W, kHeaderH });
    _header.setPosition({ 0.f, 0.f });
    _title.setPosition({ 12.f, 9.f });

    switch (_layout) {
    case MonitorLayout::Split:
        _admin.disableFlag(ml::Flag::HIDDEN);
        _student.disableFlag(ml::Flag::HIDDEN);
        _admin.setPosition({ 0.f, top });
        _admin.setSize({ half, full });
        _student.setPosition({ half, top });
        _student.setSize({ half, full });
        break;

    case MonitorLayout::AdminOnly:
        _student.enableFlag(ml::Flag::HIDDEN);
        _admin.disableFlag(ml::Flag::HIDDEN);
        _admin.setPosition({ 0.f, top });
        _admin.setSize({ W, full });
        break;

    case MonitorLayout::StudentOnly:
        _admin.enableFlag(ml::Flag::HIDDEN);
        _student.disableFlag(ml::Flag::HIDDEN);
        _student.setPosition({ 0.f, top });
        _student.setSize({ W, full });
        break;
    }
}

void MonitorApp::onReady()
{
    _admin.start();
    _student.start();
}
