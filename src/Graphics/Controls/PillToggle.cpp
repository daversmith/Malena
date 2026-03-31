// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/PillToggle.h>
#include <algorithm>
#include <cmath>

namespace ml
{
    PillToggle::PillToggle()
    {
        PillTheme::applyFrom(ThemeManager::get());
        syncFromSettings();

        onHover([this]{
            if (checkFlag(Flag::DISABLED)) return;
            setState(State::HOVERED);
        });
        onUnhover([this]{
            if (checkFlag(Flag::DISABLED)) return;
            setState(checkFlag(Flag::ON) ? State::ON : State::IDLE);
        });
        onClick([this]{
            if (checkFlag(Flag::DISABLED)) return;
            toggle();
        });
        onUpdate([this]{
            const float dt    = _animClock.restart().asSeconds();
            const float speed = animSpeed > 0.f ? animSpeed : 1000.f;
            if (std::abs(_thumbX - _thumbTarget) > 0.001f)
                _thumbX += (_thumbTarget - _thumbX) * std::min(1.f, speed * dt);
            else
                _thumbX = _thumbTarget;
            updateThumbPosition();
        });
    }

    void PillToggle::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        PillTheme::applyFrom(theme);   // theme layer only — settings untouched
        syncFromSettings();
    }

    void PillToggle::syncFromSettings()
    {
        updateThumbPosition();
        _thumb.setFillColor(checkFlag(Flag::DISABLED) ? thumbDisabledColor : thumbColor);
    }

    void PillToggle::updateThumbPosition()
    {
        const float r    = trackSize.y / 2.f;
        const float tR   = r - thumbMargin;
        const float offX = _position.x + thumbMargin + tR;
        const float onX  = _position.x + trackSize.x - thumbMargin - tR;
        const float curX = offX + (_thumbX * (onX - offX));
        _thumb.setRadius(tR);
        _thumb.setOrigin({tR, tR});
        _thumb.setPosition({curX, _position.y + r});
    }

    void PillToggle::drawCapsule(sf::RenderTarget&       target,
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

    void PillToggle::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        const bool on  = checkFlag(Flag::ON);
        const bool dis = checkFlag(Flag::DISABLED);

        if (!leftLabel.empty())
        {
            sf::Text t(*font, leftLabel, fontSize);
            t.setFillColor(dis ? disabledTextColor : textColor);
            const sf::FloatRect lb = t.getLocalBounds();
            t.setPosition({
                _position.x - lb.size.x - lb.position.x - labelOffset,
                _position.y + (trackSize.y - lb.size.y) / 2.f - lb.position.y
            });
            target.draw(t, states);
        }

        const sf::Color trackCol = dis ? disabledColor : (on ? trackOnColor : trackOffColor);
        drawCapsule(target, states, _position, trackSize, trackSize.y / 2.f, trackCol);

        if (!pillOffLabel.empty() || !pillOnLabel.empty())
        {
            const float halfW = trackSize.x / 2.f;
            const unsigned int insideSize = static_cast<unsigned int>(fontSize * 0.75f);
            auto drawInside = [&](const std::string& str, float cx, bool active)
            {
                if (str.empty()) return;
                sf::Text t(*font, str, insideSize);
                t.setFillColor(active ? pillLabelActiveColor : pillLabelInactiveColor);
                const sf::FloatRect lb = t.getLocalBounds();
                t.setPosition({
                    _position.x + cx - lb.size.x / 2.f - lb.position.x,
                    _position.y + (trackSize.y - lb.size.y) / 2.f - lb.position.y
                });
                target.draw(t, states);
            };
            drawInside(pillOffLabel, halfW * 0.5f, !on);
            drawInside(pillOnLabel,  halfW * 1.5f,  on);
        }

        target.draw(_thumb, states);

        if (!rightLabel.empty())
        {
            sf::Text t(*font, rightLabel, fontSize);
            t.setFillColor(dis ? disabledTextColor : textColor);
            const sf::FloatRect lb = t.getLocalBounds();
            t.setPosition({
                _position.x + trackSize.x + labelOffset,
                _position.y + (trackSize.y - lb.size.y) / 2.f - lb.position.y
            });
            target.draw(t, states);
        }
    }

    void PillToggle::setOn(bool on)
    {
        if (on == checkFlag(Flag::ON)) return;
        if (on) { enableFlag(Flag::ON);  setState(State::ON); }
        else    { disableFlag(Flag::ON); setState(State::IDLE); }
        _thumbTarget = on ? 1.f : 0.f;
        syncFromSettings();
        if (_onToggled) _onToggled(on);
    }

    void PillToggle::toggle()          { setOn(!checkFlag(Flag::ON)); }
    bool PillToggle::isOn()      const { return checkFlag(Flag::ON); }

    void PillToggle::setEnabled(bool e)
    {
        if (e) { disableFlag(Flag::DISABLED); setState(State::IDLE); }
        else   { enableFlag(Flag::DISABLED);  setState(State::DISABLED); }
        syncFromSettings();
    }
    bool PillToggle::isEnabled() const { return !checkFlag(Flag::DISABLED); }

    void PillToggle::onToggled(std::function<void(bool)> cb) { _onToggled = std::move(cb); }

    void PillToggle::setPosition(const sf::Vector2f& pos)
    {
        _position = {std::round(pos.x), std::round(pos.y)};
        syncFromSettings();
    }

    sf::Vector2f PillToggle::getPosition() const { return _position; }

    sf::FloatRect PillToggle::getGlobalBounds() const
    {
        float left   = _position.x;
        float top    = _position.y;
        float right  = _position.x + trackSize.x;
        float bottom = _position.y + trackSize.y;

        if (!leftLabel.empty())
        {
            sf::Text t(*font, leftLabel, fontSize);
            const sf::FloatRect lb = t.getLocalBounds();
            left = std::min(left, _position.x - lb.size.x - labelOffset);
        }
        if (!rightLabel.empty())
        {
            sf::Text t(*font, rightLabel, fontSize);
            const sf::FloatRect lb = t.getLocalBounds();
            right = std::max(right, _position.x + trackSize.x + labelOffset + lb.size.x);
        }
        return sf::FloatRect{{left, top}, {right - left, bottom - top}};
    }

} // namespace ml
