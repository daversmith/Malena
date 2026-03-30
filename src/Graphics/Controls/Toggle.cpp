// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/Toggle.h>
#include <SFML/Graphics/ConvexShape.hpp>
#include <algorithm>
#include <cmath>

namespace ml
{
    // =========================================================================
    // Toggle
    // =========================================================================

    Toggle::Toggle(const sf::Font& font)
        : _font(&font),
          _leftLabel(font),
          _rightLabel(font),
          _buttonText(font)
    {
        _leftLabel.setCharacterSize(_charSize);
        _rightLabel.setCharacterSize(_charSize);
        _buttonText.setCharacterSize(_charSize);

        layout();
        applyVisualState();

        // ── Hover ─────────────────────────────────────────────────────────────
        onHover([this]{
            if (checkFlag(Flag::DISABLED)) return;
            if (!checkFlag(Flag::ON)) setState(State::HOVERED);
            applyVisualState();
        });

        onUnhover([this]{
            if (checkFlag(Flag::DISABLED)) return;
            setState(checkFlag(Flag::ON) ? State::ON : State::IDLE);
            applyVisualState();
        });

        // ── Click ─────────────────────────────────────────────────────────────
        onClick([this]{
            if (checkFlag(Flag::DISABLED)) return;
            toggle();
        });

        // ── Update — animate pill thumb ───────────────────────────────────────
        onUpdate([this]{
            if (_style != Style::PILL) return;
            const float dt = _animClock.restart().asSeconds();
            if (std::abs(_thumbX - _thumbTarget) > 0.001f)
            {
                _thumbX += (_thumbTarget - _thumbX) *
                           std::min(1.f, (_animSpeed > 0.f ? _animSpeed : 1000.f) * dt);
            }
            else
            {
                _thumbX = _thumbTarget;
            }
            updateThumbPosition();
        });
    }

    // ── Internal helpers ──────────────────────────────────────────────────────

    void Toggle::layout()
    {
        const float trackRadius = _trackSize.y / 2.f;
        const float thumbRadius = trackRadius - _thumbMargin;

        switch (_style)
        {
            case Style::PILL:
            {
                _track.setSize(_trackSize);
                _track.setPosition(_position);
                _thumb.setRadius(thumbRadius);
                _thumb.setOrigin({thumbRadius, thumbRadius});
                updateThumbPosition();
                break;
            }
            case Style::CHECKBOX:
            {
                _box.setSize(_boxSize);
                _box.setPosition(_position);
                _box.setOutlineThickness(_boxThickness);
                break;
            }
            case Style::BUTTON:
            {
                _button.setSize(_buttonSize);
                _button.setPosition(_position);
                _button.setOutlineThickness(_buttonThick);
                break;
            }
        }

        // Widget bounds for left/right label placement
        const sf::FloatRect wb = widgetBounds();

        // Left label — sits to the left of the widget
        if (!_leftLabel.getString().isEmpty())
        {
            const sf::FloatRect lb = _leftLabel.getLocalBounds();
            _leftLabel.setPosition({
                wb.position.x - lb.size.x - lb.position.x - _labelOffset,
                wb.position.y + (wb.size.y - lb.size.y) / 2.f - lb.position.y
            });
        }

        // Right label — sits to the right of the widget
        if (!_rightLabel.getString().isEmpty())
        {
            const sf::FloatRect lb = _rightLabel.getLocalBounds();
            _rightLabel.setPosition({
                wb.position.x + wb.size.x + _labelOffset,
                wb.position.y + (wb.size.y - lb.size.y) / 2.f - lb.position.y
            });
        }

        // Button label — centred inside button
        if (_style == Style::BUTTON)
        {
            const std::string& str = checkFlag(Flag::ON) ? _buttonOnLabel : _buttonOffLabel;
            _buttonText.setString(str);
            const sf::FloatRect lb = _buttonText.getLocalBounds();
            _buttonText.setPosition({
                _position.x + (_buttonSize.x - lb.size.x) / 2.f - lb.position.x,
                _position.y + (_buttonSize.y - lb.size.y) / 2.f - lb.position.y
            });
        }
    }

    void Toggle::applyVisualState()
    {
        const bool on       = checkFlag(Flag::ON);
        const bool disabled = checkFlag(Flag::DISABLED);

        switch (_style)
        {
            case Style::PILL:
                _track.setFillColor(on ? _trackOnColor : _trackOffColor);
                _thumb.setFillColor(disabled ? sf::Color(220,220,220) : _thumbColor);
                break;

            case Style::CHECKBOX:
                _box.setFillColor(on ? _boxOnColor : _boxOffColor);
                _box.setOutlineColor(_boxBorderColor);
                break;

            case Style::BUTTON:
            {
                const std::string& str = on ? _buttonOnLabel : _buttonOffLabel;
                _buttonText.setString(str);
                _button.setFillColor(on ? _buttonOnColor : _buttonOffColor);
                _button.setOutlineColor(_buttonBorder);
                _buttonText.setFillColor(on ? _buttonOnTextColor : _buttonOffTextColor);

                // Re-centre button text
                const sf::FloatRect lb = _buttonText.getLocalBounds();
                _buttonText.setPosition({
                    _position.x + (_buttonSize.x - lb.size.x) / 2.f - lb.position.x,
                    _position.y + (_buttonSize.y - lb.size.y) / 2.f - lb.position.y
                });
                break;
            }
        }

        const sf::Color lc = disabled ? _labelDisColor : _labelColor;
        _leftLabel.setFillColor(lc);
        _rightLabel.setFillColor(lc);
    }

    void Toggle::updateThumbPosition()
    {
        const float trackRadius = _trackSize.y / 2.f;
        const float thumbRadius = trackRadius - _thumbMargin;
        const float offX = _position.x + _thumbMargin + thumbRadius;
        const float onX  = _position.x + _trackSize.x - _thumbMargin - thumbRadius;
        const float curX = offX + (_thumbX * (onX - offX));
        _thumb.setRadius(thumbRadius);
        _thumb.setOrigin({thumbRadius, thumbRadius});
        _thumb.setPosition({curX, _position.y + trackRadius});
    }

    sf::FloatRect Toggle::widgetBounds() const
    {
        switch (_style)
        {
            case Style::PILL:     return _track.getGlobalBounds();
            case Style::CHECKBOX: return _box.getGlobalBounds();
            case Style::BUTTON:   return _button.getGlobalBounds();
        }
        return {};
    }

    // ── Draw ──────────────────────────────────────────────────────────────────

    void Toggle::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (!_leftLabel.getString().isEmpty())
            target.draw(_leftLabel, states);
        if (!_rightLabel.getString().isEmpty())
            target.draw(_rightLabel, states);

        switch (_style)
        {
            case Style::PILL:     drawPill(target, states);     break;
            case Style::CHECKBOX: drawCheckbox(target, states); break;
            case Style::BUTTON:   drawButton(target, states);   break;
        }
    }

    void Toggle::drawPill(sf::RenderTarget& target,
                          const sf::RenderStates& states) const
    {
        // Draw track as a capsule — two circles + a rectangle
        const float r = _trackSize.y / 2.f;
        const sf::Color col = _track.getFillColor();

        // Left cap
        sf::CircleShape cap(r);
        cap.setFillColor(col);
        cap.setOrigin({r, r});
        cap.setPosition({_position.x + r, _position.y + r});
        target.draw(cap, states);

        // Right cap
        cap.setPosition({_position.x + _trackSize.x - r, _position.y + r});
        target.draw(cap, states);

        // Middle rect
        sf::RectangleShape mid({_trackSize.x - r * 2.f, _trackSize.y});
        mid.setFillColor(col);
        mid.setPosition({_position.x + r, _position.y});
        target.draw(mid, states);

        // Thumb
        target.draw(_thumb, states);
    }

    void Toggle::drawCheckbox(sf::RenderTarget& target,
                               const sf::RenderStates& states) const
    {
        target.draw(_box, states);
        if (checkFlag(Flag::ON))
            drawCheckmark(target, states);
    }

    void Toggle::drawButton(sf::RenderTarget& target,
                             const sf::RenderStates& states) const
    {
        // Draw button as a rounded rectangle using four circles + rects
        const sf::Color bg  = _button.getFillColor();
        const sf::Color brd = _button.getOutlineColor();
        const float     r   = _buttonRadius;
        const float     w   = _buttonSize.x;
        const float     h   = _buttonSize.y;
        const float     t   = _buttonThick;
        const float     x   = _position.x;
        const float     y   = _position.y;

        // Simple fallback — just draw rect (rounded rects need more geometry)
        sf::RectangleShape btn(_buttonSize);
        btn.setFillColor(bg);
        btn.setOutlineColor(brd);
        btn.setOutlineThickness(t);
        btn.setPosition({x, y});
        target.draw(btn, states);

        target.draw(_buttonText, states);
    }

    void Toggle::drawCheckmark(sf::RenderTarget& target,
                                const sf::RenderStates& states) const
    {
        // Draw a simple checkmark using a convex polygon (two line segments)
        const float x  = _position.x;
        const float y  = _position.y;
        const float w  = _boxSize.x;
        const float h  = _boxSize.y;
        const float pw = 2.f; // pen width

        // Tick: two rectangles rotated to form ✓
        // Leg 1: short downward-left stroke
        // Leg 2: long upward-right stroke
        // Use thin rectangles approximating the lines

        const float mx = x + w * 0.2f;
        const float my = y + h * 0.55f;
        const float ex = x + w * 0.45f;
        const float ey = y + h * 0.78f;
        const float rx = x + w * 0.8f;
        const float ry = y + h * 0.25f;

        auto drawLine = [&](float x1, float y1, float x2, float y2)
        {
            const float dx = x2 - x1;
            const float dy = y2 - y1;
            const float len = std::sqrt(dx*dx + dy*dy);
            if (len < 0.001f) return;

            sf::RectangleShape line({len, pw});
            line.setFillColor(sf::Color::White);
            line.setOrigin({0.f, pw / 2.f});
            line.setPosition({x1, y1});
            line.setRotation(sf::degrees(std::atan2(dy, dx) * 180.f / 3.14159f));
            target.draw(line, states);
        };

        drawLine(mx, my, ex, ey);
        drawLine(ex, ey, rx, ry);
    }

    // ── Style ─────────────────────────────────────────────────────────────────

    void Toggle::setStyle(Style style)
    {
        _style = style;
        layout();
        applyVisualState();
    }

    Toggle::Style Toggle::getStyle() const { return _style; }

    // ── State ─────────────────────────────────────────────────────────────────

    void Toggle::setOn(bool on)
    {
        if (on == checkFlag(Flag::ON)) return;

        if (on) { enableFlag(Flag::ON);  setState(State::ON); }
        else    { disableFlag(Flag::ON); setState(State::IDLE); }

        _thumbTarget = on ? 1.f : 0.f;
        applyVisualState();

        if (_onToggled) _onToggled(on);
    }

    void Toggle::toggle() { setOn(!checkFlag(Flag::ON)); }
    bool Toggle::isOn()  const { return checkFlag(Flag::ON); }

    // ── Enabled / disabled ────────────────────────────────────────────────────

    void Toggle::setEnabled(bool e)
    {
        if (e) { disableFlag(Flag::DISABLED); setState(State::IDLE); }
        else   { enableFlag(Flag::DISABLED);  setState(State::DISABLED); }
        applyVisualState();
    }

    bool Toggle::isEnabled() const { return !checkFlag(Flag::DISABLED); }

    // ── Labels ────────────────────────────────────────────────────────────────

    void Toggle::setLeftLabel(const std::string& text)
    { _leftLabel.setString(text); layout(); }

    void Toggle::setRightLabel(const std::string& text)
    { _rightLabel.setString(text); layout(); }

    std::string Toggle::getLeftLabel()  const { return _leftLabel.getString();  }
    std::string Toggle::getRightLabel() const { return _rightLabel.getString(); }

    void Toggle::setLabelColor(const sf::Color& c) { _labelColor = c; applyVisualState(); }
    void Toggle::setLabelOffset(float o) { _labelOffset = o; layout(); }

    // ── PILL styling ──────────────────────────────────────────────────────────

    void Toggle::setTrackSize(const sf::Vector2f& s) { _trackSize = s; layout(); applyVisualState(); }
    void Toggle::setThumbMargin(float m)              { _thumbMargin = m; layout(); }
    void Toggle::setTrackOnColor(const sf::Color& c)  { _trackOnColor  = c; applyVisualState(); }
    void Toggle::setTrackOffColor(const sf::Color& c) { _trackOffColor = c; applyVisualState(); }
    void Toggle::setThumbColor(const sf::Color& c)    { _thumbColor    = c; applyVisualState(); }
    void Toggle::setAnimationSpeed(float s)           { _animSpeed = s; }

    // ── CHECKBOX styling ──────────────────────────────────────────────────────

    void Toggle::setBoxSize(const sf::Vector2f& s)    { _boxSize = s; layout(); }
    void Toggle::setBoxOnColor(const sf::Color& c)    { _boxOnColor     = c; applyVisualState(); }
    void Toggle::setBoxOffColor(const sf::Color& c)   { _boxOffColor    = c; applyVisualState(); }
    void Toggle::setBoxBorderColor(const sf::Color& c){ _boxBorderColor = c; applyVisualState(); }
    void Toggle::setBoxBorderThickness(float t)       { _boxThickness   = t; layout(); }
    void Toggle::setBoxRadius(float r)                { _boxRadius = r; }

    // ── BUTTON styling ────────────────────────────────────────────────────────

    void Toggle::setSize(const sf::Vector2f& s)
    { _buttonSize = s; layout(); applyVisualState(); }

    void Toggle::setButtonOnLabel(const std::string& l)
    { _buttonOnLabel = l; applyVisualState(); }

    void Toggle::setButtonOffLabel(const std::string& l)
    { _buttonOffLabel = l; applyVisualState(); }

    void Toggle::setButtonOnColor(const sf::Color& c)      { _buttonOnColor      = c; applyVisualState(); }
    void Toggle::setButtonOffColor(const sf::Color& c)     { _buttonOffColor     = c; applyVisualState(); }
    void Toggle::setButtonOnTextColor(const sf::Color& c)  { _buttonOnTextColor  = c; applyVisualState(); }
    void Toggle::setButtonOffTextColor(const sf::Color& c) { _buttonOffTextColor = c; applyVisualState(); }
    void Toggle::setButtonBorderColor(const sf::Color& c)  { _buttonBorder       = c; applyVisualState(); }
    void Toggle::setButtonBorderThickness(float t)         { _buttonThick = t; layout(); }
    void Toggle::setButtonRadius(float r)                  { _buttonRadius = r; }

    // ── Font ──────────────────────────────────────────────────────────────────

    void Toggle::setFont(const sf::Font& font)
    {
        _font = &font;
        _leftLabel.setFont(font);
        _rightLabel.setFont(font);
        _buttonText.setFont(font);
        layout();
    }

    void Toggle::setCharacterSize(unsigned int size)
    {
        _charSize = size;
        _leftLabel.setCharacterSize(size);
        _rightLabel.setCharacterSize(size);
        _buttonText.setCharacterSize(size);
        layout();
    }

    unsigned int Toggle::getCharacterSize() const { return _charSize; }

    // ── Callback ─────────────────────────────────────────────────────────────

    void Toggle::onToggled(std::function<void(bool)> cb) { _onToggled = std::move(cb); }

    // ── Positionable ──────────────────────────────────────────────────────────

    void Toggle::setPosition(const sf::Vector2f& pos)
    {
        _position = pos;
        layout();
    }

    sf::Vector2f Toggle::getPosition() const { return _position; }

    sf::FloatRect Toggle::getGlobalBounds() const
    {
        const sf::FloatRect wb = widgetBounds();

        float left   = wb.position.x;
        float top    = wb.position.y;
        float right  = wb.position.x + wb.size.x;
        float bottom = wb.position.y + wb.size.y;

        auto expand = [&](const sf::Text& label){
            if (label.getString().isEmpty()) return;
            const sf::FloatRect lb = label.getGlobalBounds();
            left   = std::min(left,   lb.position.x);
            top    = std::min(top,    lb.position.y);
            right  = std::max(right,  lb.position.x + lb.size.x);
            bottom = std::max(bottom, lb.position.y + lb.size.y);
        };

        expand(_leftLabel);
        expand(_rightLabel);

        return sf::FloatRect{{left, top}, {right - left, bottom - top}};
    }

    // =========================================================================
    // ToggleGroup
    // =========================================================================

    ToggleGroup::ToggleGroup(const sf::Font& font)
        : _font(&font)
    {}

    void ToggleGroup::relayout()
    {
        float y = _position.y;
        for (auto& entry : _entries)
        {
            entry.toggle->setPosition({_position.x, y});
            y += entry.toggle->getGlobalBounds().size.y + _spacing;
        }
    }

    void ToggleGroup::attachCallback(Entry& entry)
    {
        const std::string key = entry.key;
        entry.toggle->onToggled([this, key](bool on){
            if (_onAnyToggled) _onAnyToggled(key, on);
        });
    }

    // ── Draw ──────────────────────────────────────────────────────────────────

    void ToggleGroup::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        for (const auto& entry : _entries)
            target.draw(*entry.toggle, states);
    }

    // ── Adding toggles ────────────────────────────────────────────────────────

    Toggle& ToggleGroup::addToggle(const std::string& label,
                                   const std::string& key,
                                   bool               on)
    {
        auto t = std::make_unique<Toggle>(*_font);
        t->setStyle(_defaultStyle);
        t->setCharacterSize(_charSize);
        t->setRightLabel(label);
        if (on) t->setOn(true);

        Toggle* ptr = t.get();
        _owned.push_back(std::move(t));

        Entry entry{ptr, key, true};
        attachCallback(entry);
        _entries.push_back(std::move(entry));

        relayout();
        return *ptr;
    }

    void ToggleGroup::add(Toggle& toggle, const std::string& key)
    {
        Entry entry{&toggle, key, false};
        attachCallback(entry);
        _entries.push_back(std::move(entry));
        relayout();
    }

    void ToggleGroup::clearToggles()
    {
        _entries.clear();
        _owned.clear();
    }

    // ── State access ──────────────────────────────────────────────────────────

    bool ToggleGroup::isOn(const std::string& key) const
    {
        for (const auto& e : _entries)
            if (e.key == key) return e.toggle->isOn();
        return false;
    }

    void ToggleGroup::setOn(const std::string& key, bool on)
    {
        for (auto& e : _entries)
            if (e.key == key) { e.toggle->setOn(on); return; }
    }

    void ToggleGroup::setAllOn()
    { for (auto& e : _entries) e.toggle->setOn(true); }

    void ToggleGroup::setAllOff()
    { for (auto& e : _entries) e.toggle->setOn(false); }

    std::vector<std::string> ToggleGroup::getOnKeys() const
    {
        std::vector<std::string> keys;
        for (const auto& e : _entries)
            if (e.toggle->isOn()) keys.push_back(e.key);
        return keys;
    }

    // ── Callbacks ─────────────────────────────────────────────────────────────

    void ToggleGroup::onAnyToggled(std::function<void(const std::string&, bool)> cb)
    {
        _onAnyToggled = std::move(cb);
    }

    // ── Styling ───────────────────────────────────────────────────────────────

    void ToggleGroup::setDefaultStyle(Toggle::Style style) { _defaultStyle = style; }

    void ToggleGroup::setSpacing(float s) { _spacing = s; relayout(); }

    void ToggleGroup::setFont(const sf::Font& font)
    {
        _font = &font;
        for (auto& e : _entries) e.toggle->setFont(font);
        relayout();
    }

    void ToggleGroup::setCharacterSize(unsigned int size)
    {
        _charSize = size;
        for (auto& e : _entries) e.toggle->setCharacterSize(size);
        relayout();
    }

    // ── Positionable ──────────────────────────────────────────────────────────

    void ToggleGroup::setPosition(const sf::Vector2f& pos)
    {
        _position = pos;
        relayout();
    }

    sf::Vector2f ToggleGroup::getPosition() const { return _position; }

    sf::FloatRect ToggleGroup::getGlobalBounds() const
    {
        if (_entries.empty()) return {};

        sf::FloatRect bounds = _entries.front().toggle->getGlobalBounds();
        float bottom = bounds.position.y + bounds.size.y;

        for (std::size_t i = 1; i < _entries.size(); ++i)
        {
            const sf::FloatRect b = _entries[i].toggle->getGlobalBounds();
            bounds.size.x = std::max(bounds.size.x, b.size.x);
            bottom = std::max(bottom, b.position.y + b.size.y);
        }

        bounds.size.y = bottom - bounds.position.y;
        return bounds;
    }

} // namespace ml
