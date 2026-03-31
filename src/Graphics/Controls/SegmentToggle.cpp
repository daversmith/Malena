// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/SegmentToggle.h>
#include <algorithm>
#include <cmath>

namespace ml
{
    SegmentToggle::SegmentToggle()
    {
        SegmentTheme::applyFrom(ThemeManager::get());
        syncFromSettings();

        onHover([this]{ if (!checkFlag(Flag::DISABLED)) setState(State::HOVERED); });
        onUnhover([this]{ if (!checkFlag(Flag::DISABLED)) setState(checkFlag(Flag::ON) ? State::ON : State::IDLE); });
        onClick([this]{ if (!checkFlag(Flag::DISABLED)) toggle(); });
        onUpdate([this]{
            const float dt    = _animClock.restart().asSeconds();
            const float speed = animSpeed > 0.f ? animSpeed : 1000.f;
            if (std::abs(_thumbX - _thumbTarget) > 0.001f)
                _thumbX += (_thumbTarget - _thumbX) * std::min(1.f, speed * dt);
            else
                _thumbX = _thumbTarget;
        });
    }

    void SegmentToggle::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        SegmentTheme::applyFrom(theme);
        syncFromSettings();
    }

    void SegmentToggle::syncFromSettings() {}

    void SegmentToggle::drawCapsule(sf::RenderTarget& target,
                                    const sf::RenderStates& states,
                                    sf::Vector2f pos, sf::Vector2f sz,
                                    float r, sf::Color fill) const
    {
        r = std::min(r, sz.y / 2.f);
        sf::CircleShape cap(r);
        cap.setFillColor(fill);
        cap.setOrigin({r, r});
        cap.setPosition({pos.x + r, pos.y + r});
        target.draw(cap, states);
        cap.setPosition({pos.x + sz.x - r, pos.y + r});
        target.draw(cap, states);
        sf::RectangleShape mid({sz.x - r * 2.f, sz.y});
        mid.setFillColor(fill);
        mid.setPosition({pos.x + r, pos.y});
        target.draw(mid, states);
    }

    void SegmentToggle::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        const float w = size.x, h = size.y;
        const float r = std::min(segRadius, h / 2.f);
        const float p = segPadding;

        drawCapsule(target, states, _position, size, r, trackColor);

        const float tW    = w / 2.f - p;
        const float tH    = h - p * 2.f;
        const float tR    = std::min(r - p, tH / 2.f);
        const float offTX = _position.x + p;
        const float onTX  = _position.x + w / 2.f;
        const float curTX = offTX + (_thumbX * (onTX - offTX));
        drawCapsule(target, states, {curTX, _position.y + p}, {tW, tH}, tR, thumbColor);

        const bool on = checkFlag(Flag::ON);
        auto drawLabel = [&](const std::string& str, float cx, bool active)
        {
            sf::Text t(*font, str, fontSize);
            t.setFillColor(active ? activeTextColor : inactiveTextColor);
            const sf::FloatRect lb = t.getLocalBounds();
            t.setPosition({
                _position.x + cx - lb.size.x / 2.f - lb.position.x,
                _position.y + (h - lb.size.y) / 2.f - lb.position.y
            });
            target.draw(t, states);
        };
        drawLabel(offLabel, w * 0.25f, !on);
        drawLabel(onLabel,  w * 0.75f,  on);
    }

    void SegmentToggle::setOn(bool on)
    {
        if (on == checkFlag(Flag::ON)) return;
        if (on) { enableFlag(Flag::ON);  setState(State::ON); }
        else    { disableFlag(Flag::ON); setState(State::IDLE); }
        _thumbTarget = on ? 1.f : 0.f;
        if (_onToggled) _onToggled(on);
    }
    void SegmentToggle::toggle()          { setOn(!checkFlag(Flag::ON)); }
    bool SegmentToggle::isOn()      const { return checkFlag(Flag::ON); }
    void SegmentToggle::setEnabled(bool e)
    {
        if (e) { disableFlag(Flag::DISABLED); setState(State::IDLE); }
        else   { enableFlag(Flag::DISABLED);  setState(State::DISABLED); }
    }
    bool SegmentToggle::isEnabled() const { return !checkFlag(Flag::DISABLED); }

    void SegmentToggle::onToggled(std::function<void(bool)> cb) { _onToggled = std::move(cb); }

    void SegmentToggle::setPosition(const sf::Vector2f& pos)
    { _position = {std::round(pos.x), std::round(pos.y)}; }
    sf::Vector2f  SegmentToggle::getPosition()     const { return _position; }
    sf::FloatRect SegmentToggle::getGlobalBounds() const { return sf::FloatRect{_position, size}; }

} // namespace ml
