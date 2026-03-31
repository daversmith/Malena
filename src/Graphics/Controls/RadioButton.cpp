// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/RadioButton.h>
#include <Malena/Utilities/Align.h>
#include <algorithm>
#include <cmath>

namespace ml
{
    RadioButton::RadioButton(const std::string& label, const sf::Font& font_)
        : _label(font_)
    {
        RadioButtonTheme::applyFrom(ThemeManager::get());
        this->font = &font_;

        _ring.setFillColor(sf::Color::Transparent);
        _ring.setOutlineThickness(ringThickness);
        _dot.setFillColor(dotColor);
        _dot.setOutlineThickness(0.f);

        _labelStr = label;
        _label.setString(label);
        _label.setCharacterSize(fontSize);
        _label.setFillColor(labelColor);

        onHover([this]{
            if (!checkFlag(Flag::DISABLED) && !checkFlag(Flag::SELECTED))
            {
                setState(State::HOVERED);
                applyVisualState();
            }
        });
        onUnhover([this]{
            if (!checkFlag(Flag::DISABLED) && !checkFlag(Flag::SELECTED))
            {
                setState(State::IDLE);
                applyVisualState();
            }
        });

        setState(State::IDLE);
        layout();
        applyVisualState();
    }

    void RadioButton::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        RadioButtonTheme::applyFrom(theme);
        _label.setCharacterSize(fontSize);
        applyVisualState();
        layout();
    }

    void RadioButton::layout()
    {
        _ring.setRadius(radius);
        _ring.setOrigin({radius, radius});
        _ring.setPosition(_position + sf::Vector2f{radius, radius});
        _ring.setOutlineThickness(ringThickness);

        const float dotRadius = radius * std::clamp(dotScale, 0.1f, 0.9f);
        _dot.setRadius(dotRadius);
        _dot.setOrigin({dotRadius, dotRadius});
        _dot.setPosition(_ring.getPosition());

        _label.setFont(*font);
        _label.setCharacterSize(fontSize);
        Align::setRightOf(_ring, _label, labelOffset);
        Align::centerVertically(_ring, _label);
    }

    void RadioButton::applyVisualState()
    {
        const bool disabled = checkFlag(Flag::DISABLED);
        const bool selected = checkFlag(Flag::SELECTED);

        if (disabled)
        {
            _ring.setOutlineColor(ringDisabledColor);
            _dot.setFillColor(dotDisabledColor);
            _label.setFillColor(labelDisabledColor);
        }
        else if (selected)
        {
            _ring.setOutlineColor(ringSelectedColor);
            _dot.setFillColor(dotColor);
            _label.setFillColor(labelColor);
        }
        else if (isState(State::HOVERED))
        {
            _ring.setOutlineColor(ringHoverColor);
            _label.setFillColor(labelColor);
        }
        else
        {
            _ring.setOutlineColor(ringColor);
            _label.setFillColor(labelColor);
        }
    }

    void RadioButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(_ring, states);
        if (checkFlag(Flag::SELECTED))
            target.draw(_dot, states);
        target.draw(_label, states);
    }

    void RadioButton::select()
    {
        if (checkFlag(Flag::DISABLED)) return;
        enableFlag(Flag::SELECTED);
        setState(State::SELECTED);
        applyVisualState();
        Selectable::select();
    }

    void RadioButton::deselect()
    {
        disableFlag(Flag::SELECTED);
        setState(State::IDLE);
        applyVisualState();
        Selectable::deselect();
    }

    bool RadioButton::isSelected() const { return checkFlag(Flag::SELECTED); }

    void RadioButton::setEnabled(bool enabled)
    {
        if (enabled) { disableFlag(Flag::DISABLED); setState(State::IDLE); }
        else         { enableFlag(Flag::DISABLED);  setState(State::DISABLED); }
        applyVisualState();
    }

    bool RadioButton::isEnabled() const { return !checkFlag(Flag::DISABLED); }

    void RadioButton::setLabel(const std::string& label)
    {
        _labelStr = label;
        _label.setString(label);
        layout();
    }

    std::string RadioButton::getLabel() const { return _labelStr; }

    void RadioButton::setPosition(const sf::Vector2f& pos)
    {
        _position = pos;
        layout();
    }

    sf::Vector2f RadioButton::getPosition() const { return _position; }

    sf::FloatRect RadioButton::getGlobalBounds() const
    {
        const sf::FloatRect lb = _label.getGlobalBounds();
        return sf::FloatRect{
            {_position.x, _position.y},
            {lb.position.x + lb.size.x - _position.x, radius * 2.f}
        };
    }

} // namespace ml
