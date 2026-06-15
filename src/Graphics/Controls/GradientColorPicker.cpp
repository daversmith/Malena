// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#include <Malena/Graphics/Controls/GradientColorPicker.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <Malena/Engine/App/AppManager.h>   // exclusive-owner (modal) input gating

#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>

#include <algorithm>
#include <cmath>

namespace ml
{
    // ── HSV / RGB ─────────────────────────────────────────────────────────────
    sf::Color GradientColorPicker::hsvToRgb(float h, float s, float v)
    {
        h = std::fmod(std::fmod(h, 360.f) + 360.f, 360.f);
        const float c = v * s;
        const float x = c * (1.f - std::fabs(std::fmod(h / 60.f, 2.f) - 1.f));
        const float m = v - c;
        float r = 0, g = 0, b = 0;
        if      (h <  60) { r = c; g = x; }
        else if (h < 120) { r = x; g = c; }
        else if (h < 180) { g = c; b = x; }
        else if (h < 240) { g = x; b = c; }
        else if (h < 300) { r = x; b = c; }
        else              { r = c; b = x; }
        auto u8 = [](float f){ return static_cast<std::uint8_t>(std::lround(f * 255.f)); };
        return { u8(r + m), u8(g + m), u8(b + m) };
    }

    void GradientColorPicker::rgbToHsv(const sf::Color& col, float& h, float& s, float& v)
    {
        const float r = col.r / 255.f, g = col.g / 255.f, b = col.b / 255.f;
        const float mx = std::max({ r, g, b }), mn = std::min({ r, g, b });
        const float d = mx - mn;
        v = mx;
        s = (mx <= 0.f) ? 0.f : d / mx;
        if (d <= 0.f)       h = 0.f;
        else if (mx == r)   h = 60.f * std::fmod((g - b) / d, 6.f);
        else if (mx == g)   h = 60.f * (((b - r) / d) + 2.f);
        else                h = 60.f * (((r - g) / d) + 4.f);
        if (h < 0.f) h += 360.f;
    }

    // ── Construction ────────────────────────────────────────────────────────────
    GradientColorPicker::GradientColorPicker()
    {
        this->setFillColor(sf::Color::Transparent);

        onUpdate([this] {
            if (checkFlag(ml::Flag::HIDDEN) || !AppManager::isUnderExclusiveOwner(this))
            { _drag = 0; _prevMouseDown = false; return; }

            const auto& win = WindowManager::getWindow();
            const sf::Vector2f wp = win.mapPixelToCoords(sf::Mouse::getPosition(win));
            const bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

            const sf::FloatRect sv  = svRect();
            const sf::FloatRect hue = hueRect();

            // Hue grab area is the strip plus a margin for the triangle handles
            // (incl. vertical margin so the handle is grabbable at the very
            // top/bottom, where the triangle overhangs the strip ends).
            const sf::FloatRect hueHit{
                { hue.position.x - 10.f, hue.position.y - 8.f },
                { hue.size.x + 20.f,     hue.size.y + 16.f } };

            if (down && !_prevMouseDown)
            {
                if      (sv.contains(wp))    _drag = 1;
                else if (hueHit.contains(wp)) _drag = 2;
            }

            if (down && _drag == 1)
            {
                _s = std::clamp((wp.x - sv.position.x) / sv.size.x, 0.f, 1.f);
                _v = std::clamp(1.f - (wp.y - sv.position.y) / sv.size.y, 0.f, 1.f);
                emitChange();
            }
            else if (down && _drag == 2)
            {
                _h = std::clamp((wp.y - hue.position.y) / hue.size.y, 0.f, 1.f) * 360.f;
                emitChange();
            }

            if (!down) _drag = 0;
            _prevMouseDown = down;

            // Enter = commit/select the current color.
            const bool enter = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter);
            if (enter && !_prevEnter && _onCommit) _onCommit();
            _prevEnter = enter;
        });
    }

    void GradientColorPicker::emitChange()
    {
        if (_onColorChanged) _onColorChanged(color());
    }

    // ── Accessors ───────────────────────────────────────────────────────────────
    sf::Color GradientColorPicker::color() const { return hsvToRgb(_h, _s, _v); }

    void GradientColorPicker::setColor(const sf::Color& c) { rgbToHsv(c, _h, _s, _v); }

    void GradientColorPicker::onColorChanged(std::function<void(const sf::Color&)> cb)
    { _onColorChanged = std::move(cb); }

    void GradientColorPicker::onCommit(std::function<void()> cb)
    { _onCommit = std::move(cb); }

    void GradientColorPicker::setSize(const sf::Vector2f& size)
    { _size = size; Graphic<sf::RectangleShape>::setSize(size); }

    sf::Vector2f GradientColorPicker::getSize() const { return _size; }

    void GradientColorPicker::setPosition(const sf::Vector2f& pos)
    { _pos = pos; Graphic<sf::RectangleShape>::setPosition(pos); }

    sf::Vector2f  GradientColorPicker::getPosition()     const { return _pos; }
    sf::FloatRect GradientColorPicker::getGlobalBounds() const { return { _pos, _size }; }

    sf::FloatRect GradientColorPicker::svRect() const
    {
        return { _pos, { _size.x - _hueBarW - _gap, _size.y } };
    }

    sf::FloatRect GradientColorPicker::hueRect() const
    {
        return { { _pos.x + _size.x - _hueBarW, _pos.y }, { _hueBarW, _size.y } };
    }

    // ── Drawing ───────────────────────────────────────────────────────────────
    void GradientColorPicker::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        const sf::FloatRect sv  = svRect();
        const sf::FloatRect hue = hueRect();

        // SV square — bilinear quad: TL white, TR pure-hue, bottom black.
        const sf::Color pureHue = hsvToRgb(_h, 1.f, 1.f);
        sf::VertexArray svQuad(sf::PrimitiveType::TriangleStrip, 4);
        svQuad[0].position = sv.position;                                   // TL
        svQuad[0].color    = sf::Color::White;
        svQuad[1].position = { sv.position.x, sv.position.y + sv.size.y };  // BL
        svQuad[1].color    = sf::Color::Black;
        svQuad[2].position = { sv.position.x + sv.size.x, sv.position.y };  // TR
        svQuad[2].color    = pureHue;
        svQuad[3].position = { sv.position.x + sv.size.x, sv.position.y + sv.size.y }; // BR
        svQuad[3].color    = sf::Color::Black;
        target.draw(svQuad, states);

        // Hue bar — vertical rainbow in 6 segments.
        sf::VertexArray hueStrip(sf::PrimitiveType::TriangleStrip, 14);
        for (int i = 0; i <= 6; ++i)
        {
            const float t = i / 6.f;
            const float y = hue.position.y + t * hue.size.y;
            const sf::Color c = hsvToRgb(t * 360.f, 1.f, 1.f);
            hueStrip[i * 2 + 0].position = { hue.position.x, y };
            hueStrip[i * 2 + 0].color    = c;
            hueStrip[i * 2 + 1].position = { hue.position.x + hue.size.x, y };
            hueStrip[i * 2 + 1].color    = c;
        }
        target.draw(hueStrip, states);

        // SV handle.
        sf::CircleShape h(6.f);
        h.setOrigin({ 6.f, 6.f });
        h.setPosition({ sv.position.x + _s * sv.size.x,
                        sv.position.y + (1.f - _v) * sv.size.y });
        h.setFillColor(sf::Color::Transparent);
        h.setOutlineThickness(2.f);
        h.setOutlineColor(_v > 0.5f ? sf::Color::Black : sf::Color::White);
        target.draw(h, states);

        // Hue handle — two triangles flanking the strip at the current hue,
        // pointing inward (Photoshop-style). Draggable via the widened hit area.
        const float my = hue.position.y + (_h / 360.f) * hue.size.y;
        const float ts = 7.f;   // triangle size
        auto triangle = [&](float apexX, float baseX) {
            sf::ConvexShape t(3);
            t.setPoint(0, { apexX, my });
            t.setPoint(1, { baseX, my - ts });
            t.setPoint(2, { baseX, my + ts });
            t.setFillColor(sf::Color::White);
            t.setOutlineThickness(1.f);
            t.setOutlineColor(sf::Color(40, 40, 40));
            target.draw(t, states);
        };
        const float hueRight = hue.position.x + hue.size.x;
        triangle(hue.position.x, hue.position.x - ts);  // left handle, points right ►
        triangle(hueRight,       hueRight + ts);        // right handle, points left ◄
    }

} // namespace ml
