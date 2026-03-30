// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/PillToggle.h>
#include <algorithm>
#include <cmath>

namespace ml
{
    PillToggle::PillToggle()
    {
        // Sync visual state from PillSettings defaults (which already read
        // from the active theme via ThemeManager::get() at member init)
        syncFromSettings();

        // ── Hover ─────────────────────────────────────────────────────────────
        onHover([this]{
            if (checkFlag(Flag::DISABLED)) return;
            setState(State::HOVERED);
        });

        onUnhover([this]{
            if (checkFlag(Flag::DISABLED)) return;
            setState(checkFlag(Flag::ON) ? State::ON : State::IDLE);
        });

        // ── Click ─────────────────────────────────────────────────────────────
        onClick([this]{
            if (checkFlag(Flag::DISABLED)) return;
            toggle();
        });

        // ── Update — animate thumb ────────────────────────────────────────────
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

    // ── Themeable ─────────────────────────────────────────────────────────────

    void PillToggle::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        PillSettings::applyTheme(theme); // walks the full settings chain
        syncFromSettings();
    }

    // ── syncFromSettings ──────────────────────────────────────────────────────

    void PillToggle::syncFromSettings()
    {
        // This is the single authoritative place where PillSettings values
        // are pushed to SFML shapes. Every setter and applySettings() ends
        // up here — one code path, no duplication.

        updateThumbPosition();

        // Thumb color depends on disabled state
        _thumb.setFillColor(checkFlag(Flag::DISABLED)
                            ? sf::Color(220, 220, 220) : thumbColor);

        // External labels are rebuilt lazily in draw() — no caching needed
        // since draw() creates sf::Text on the stack using current settings
    }

    void PillToggle::updateThumbPosition()
    {
        const float r      = trackSize.y / 2.f;
        const float tR     = r - thumbMargin;
        const float offX   = _position.x + thumbMargin + tR;
        const float onX    = _position.x + trackSize.x - thumbMargin - tR;
        const float curX   = offX + (_thumbX * (onX - offX));

        _thumb.setRadius(tR);
        _thumb.setOrigin({tR, tR});
        _thumb.setPosition({curX, _position.y + r});
    }

    // ── drawCapsule ───────────────────────────────────────────────────────────

    void PillToggle::drawCapsule(sf::RenderTarget&       target,
                                  const sf::RenderStates& states,
                                  sf::Vector2f            pos,
                                  sf::Vector2f            size,
                                  float                   r,
                                  sf::Color               fill) const
    {
        r = std::min(r, size.y / 2.f);

        sf::CircleShape cap(r);
        cap.setFillColor(fill);
        cap.setOrigin({r, r});

        cap.setPosition({pos.x + r, pos.y + r});
        target.draw(cap, states);

        cap.setPosition({pos.x + size.x - r, pos.y + r});
        target.draw(cap, states);

        sf::RectangleShape mid({size.x - r * 2.f, size.y});
        mid.setFillColor(fill);
        mid.setPosition({pos.x + r, pos.y});
        target.draw(mid, states);
    }

    // ── draw ──────────────────────────────────────────────────────────────────

    void PillToggle::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        const bool on = checkFlag(Flag::ON);

        // ── External left label ───────────────────────────────────────────────
        if (!leftLabel.empty())
        {
            sf::Text t(*font, leftLabel, fontSize);
            t.setFillColor(checkFlag(Flag::DISABLED) ? disabledTextColor : textColor);
            const sf::FloatRect lb = t.getLocalBounds();
            t.setPosition({
                _position.x - lb.size.x - lb.position.x - labelOffset,
                _position.y + (trackSize.y - lb.size.y) / 2.f - lb.position.y
            });
            target.draw(t, states);
        }

        // ── Track capsule ─────────────────────────────────────────────────────
        const sf::Color trackCol = checkFlag(Flag::DISABLED)
            ? disabledColor
            : (on ? trackOnColor : trackOffColor);

        drawCapsule(target, states, _position, trackSize, trackSize.y / 2.f, trackCol);

        // ── Inside labels ─────────────────────────────────────────────────────
        if (!pillOffLabel.empty() || !pillOnLabel.empty())
        {
            const float halfW    = trackSize.x / 2.f;
            const unsigned int insideSize = static_cast<unsigned int>(fontSize * 0.75f);

            auto drawInsideLabel = [&](const std::string& str, float centreX, bool active)
            {
                if (str.empty()) return;
                sf::Text t(*font, str, insideSize);
                t.setFillColor(active ? pillLabelActiveColor : pillLabelInactiveColor);
                const sf::FloatRect lb = t.getLocalBounds();
                t.setPosition({
                    _position.x + centreX - lb.size.x / 2.f - lb.position.x,
                    _position.y + (trackSize.y - lb.size.y) / 2.f - lb.position.y
                });
                target.draw(t, states);
            };

            drawInsideLabel(pillOffLabel, halfW * 0.5f, !on);
            drawInsideLabel(pillOnLabel,  halfW * 1.5f,  on);
        }

        // ── Thumb ─────────────────────────────────────────────────────────────
        target.draw(_thumb, states);

        // ── External right label ──────────────────────────────────────────────
        if (!rightLabel.empty())
        {
            sf::Text t(*font, rightLabel, fontSize);
            t.setFillColor(checkFlag(Flag::DISABLED) ? disabledTextColor : textColor);
            const sf::FloatRect lb = t.getLocalBounds();
            t.setPosition({
                _position.x + trackSize.x + labelOffset,
                _position.y + (trackSize.y - lb.size.y) / 2.f - lb.position.y
            });
            target.draw(t, states);
        }
    }

    // ── State ─────────────────────────────────────────────────────────────────

    void PillToggle::setOn(bool on)
    {
        if (on == checkFlag(Flag::ON)) return;

        if (on) { enableFlag(Flag::ON);  setState(State::ON); }
        else    { disableFlag(Flag::ON); setState(State::IDLE); }

        _thumbTarget = on ? 1.f : 0.f;
        syncFromSettings();

        if (_onToggled) _onToggled(on);
    }

    void PillToggle::toggle() { setOn(!checkFlag(Flag::ON)); }
    bool PillToggle::isOn()   const { return checkFlag(Flag::ON); }

    // ── Enabled / disabled ────────────────────────────────────────────────────

    void PillToggle::setEnabled(bool e)
    {
        if (e) { disableFlag(Flag::DISABLED); setState(State::IDLE); }
        else   { enableFlag(Flag::DISABLED);  setState(State::DISABLED); }
        syncFromSettings();
    }

    bool PillToggle::isEnabled() const { return !checkFlag(Flag::DISABLED); }

    // ── Setters — each updates one PillSettings variable then syncs ───────────

    void PillToggle::setTrackSize(const sf::Vector2f& s)
    { trackSize = s; syncFromSettings(); }

    sf::Vector2f PillToggle::getTrackSize() const { return trackSize; }

    void PillToggle::setTrackOnColor(const sf::Color& c)
    { trackOnColor = c; syncFromSettings(); }

    void PillToggle::setTrackOffColor(const sf::Color& c)
    { trackOffColor = c; syncFromSettings(); }

    void PillToggle::setThumbColor(const sf::Color& c)
    { thumbColor = c; syncFromSettings(); }

    void PillToggle::setThumbMargin(float m)
    { thumbMargin = m; syncFromSettings(); }

    void PillToggle::setInsideLabels(const std::string& off, const std::string& on)
    { pillOffLabel = off; pillOnLabel = on; }

    void PillToggle::setPillLabelColors(const sf::Color& active, const sf::Color& inactive)
    { pillLabelActiveColor = active; pillLabelInactiveColor = inactive; }

    void PillToggle::setLeftLabel(const std::string& t)   { leftLabel  = t; }
    void PillToggle::setRightLabel(const std::string& t)  { rightLabel = t; }
    std::string PillToggle::getLeftLabel()  const         { return leftLabel;  }
    std::string PillToggle::getRightLabel() const         { return rightLabel; }

    void PillToggle::setLabelColor(const sf::Color& c)
    { textColor = c; }

    void PillToggle::setLabelOffset(float o)
    { labelOffset = o; }

    void PillToggle::setAnimationSpeed(float s)
    { animSpeed = s; }

    // ── Font ──────────────────────────────────────────────────────────────────

    void PillToggle::setFont(const sf::Font& f)
    { font = &f; }

    void PillToggle::setCharacterSize(unsigned int size)
    { fontSize = size; }

    unsigned int PillToggle::getCharacterSize() const { return fontSize; }

    // ── Callback ─────────────────────────────────────────────────────────────

    void PillToggle::onToggled(std::function<void(bool)> cb)
    { _onToggled = std::move(cb); }

    // ── Positionable ──────────────────────────────────────────────────────────

    void PillToggle::setPosition(const sf::Vector2f& pos)
    {
        _position = {std::round(pos.x), std::round(pos.y)};
        syncFromSettings();
    }

    sf::Vector2f PillToggle::getPosition() const { return _position; }

    sf::FloatRect PillToggle::getGlobalBounds() const
    {
        // Start with track bounds
        float left   = _position.x;
        float top    = _position.y;
        float right  = _position.x + trackSize.x;
        float bottom = _position.y + trackSize.y;

        // Expand for left label
        if (!leftLabel.empty())
        {
            sf::Text t(*font, leftLabel, fontSize);
            const sf::FloatRect lb = t.getGlobalBounds();
            left = std::min(left, _position.x - lb.size.x - labelOffset);
        }

        // Expand for right label
        if (!rightLabel.empty())
        {
            sf::Text t(*font, rightLabel, fontSize);
            const sf::FloatRect lb = t.getLocalBounds();
            right = std::max(right, _position.x + trackSize.x + labelOffset + lb.size.x);
        }

        return sf::FloatRect{{left, top}, {right - left, bottom - top}};
    }

} // namespace ml
