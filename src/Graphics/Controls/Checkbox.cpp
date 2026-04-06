// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/Checkbox.h>
#include <Malena/Utilities/Align.h>
#include <algorithm>
#include <cmath>

namespace ml
{
    Checkbox::Checkbox(const std::string& label, const sf::Font& font_)
        : _checkmark(sf::PrimitiveType::LineStrip, 3),
          _label(font_)
    {
        // Seed from active theme, then apply the constructor font
        CheckboxTheme::applyFrom(ThemeManager::get());
        this->font = &font_;

        _box.setOutlineThickness(1.5f);
        _labelStr = label;
        _label.setString(label);
        _label.setCharacterSize(fontSize);
        _label.setFillColor(labelColor);

        onHover([this]{
            if (!checkFlag(Flag::DISABLED) && !checkFlag(Flag::CHECKED))
            {
                setState(State::HOVERED);
                applyVisualState();
            }
        });
        onUnhover([this]{
            if (!checkFlag(Flag::DISABLED) && !checkFlag(Flag::CHECKED))
            {
                setState(State::IDLE);
                applyVisualState();
            }
        });
        onClick([this]{
            if (!checkFlag(Flag::DISABLED)) toggle();
        });

        setState(State::IDLE);
        layout();
        applyVisualState();
    }

    void Checkbox::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        CheckboxTheme::applyFrom(theme);
        _label.setCharacterSize(fontSize);
        applyVisualState();
        layout();
    }

    void Checkbox::layout()
    {
        _box.setSize({boxSize, boxSize});
        _box.setPosition(_position);
        buildCheckmark();
        _label.setFont(*font);
        _label.setCharacterSize(fontSize);
        Align::setRightOf(_box, _label, labelOffset);
        Align::centerVertically(_box, _label);
    }

    void Checkbox::buildCheckmark()
    {
        const float inset = boxSize * 0.2f;
        const float x = _position.x;
        const float y = _position.y;
        _checkmark[0].position = { x + inset,            y + boxSize * 0.55f };
        _checkmark[1].position = { x + boxSize * 0.42f,  y + boxSize - inset };
        _checkmark[2].position = { x + boxSize - inset,  y + inset           };
        _checkmark[0].color = checkColor;
        _checkmark[1].color = checkColor;
        _checkmark[2].color = checkColor;
    }

    void Checkbox::applyVisualState()
    {
        const bool disabled = checkFlag(Flag::DISABLED);
        const bool checked  = checkFlag(Flag::CHECKED);

        if (disabled)
        {
            _box.setFillColor(boxDisabledColor);
            _box.setOutlineColor(boxDisabledColor);
            _label.setFillColor(labelDisabledColor);
            for (std::size_t i = 0; i < _checkmark.getVertexCount(); ++i)
                _checkmark[i].color = checkDisabledColor;
        }
        else if (checked)
        {
            _box.setFillColor(boxCheckedColor);
            _box.setOutlineColor(boxCheckedColor);
            _label.setFillColor(labelColor);
            for (std::size_t i = 0; i < _checkmark.getVertexCount(); ++i)
                _checkmark[i].color = checkColor;
        }
        else if (isState(State::HOVERED))
        {
            _box.setFillColor(boxHoverColor);
            _box.setOutlineColor(outlineColor);
            _label.setFillColor(labelColor);
        }
        else
        {
            _box.setFillColor(boxColor);
            _box.setOutlineColor(outlineColor);
            _label.setFillColor(labelColor);
        }
    }

    void Checkbox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(_box, states);
        if (checkFlag(Flag::CHECKED))
            target.draw(_checkmark, states);
        target.draw(_label, states);
    }

    void Checkbox::check()
    {
        if (checkFlag(Flag::DISABLED)) return;
        enableFlag(Flag::CHECKED);
        setState(State::CHECKED);
        applyVisualState();
        Selectable::select();
    }

    void Checkbox::uncheck()
    {
        disableFlag(Flag::CHECKED);
        setState(State::IDLE);
        applyVisualState();
        Selectable::deselect();
    }

    void Checkbox::toggle()
    {
        if (checkFlag(Flag::CHECKED)) uncheck();
        else                          check();
    }

    bool Checkbox::isChecked() const { return checkFlag(Flag::CHECKED); }

    void Checkbox::setEnabled(bool enabled)
    {
        if (enabled) { disableFlag(Flag::DISABLED); setState(State::IDLE); }
        else         { enableFlag(Flag::DISABLED);  setState(State::DISABLED); }
        applyVisualState();
    }

    bool Checkbox::isEnabled() const { return !checkFlag(Flag::DISABLED); }

    void Checkbox::setLabel(const std::string& label)
    {
        _labelStr = label;
        _label.setString(label);
        layout();
    }

    std::string Checkbox::getLabel() const { return _labelStr; }

    void Checkbox::setPosition(const sf::Vector2f& pos)
    {
        _position = pos;
        layout();
    }

    sf::Vector2f Checkbox::getPosition() const { return _position; }

    sf::FloatRect Checkbox::getGlobalBounds() const
    {
        const sf::FloatRect lb = _label.getGlobalBounds();
        return sf::FloatRect{
            {_position.x, _position.y},
            {lb.position.x + lb.size.x - _position.x, boxSize}
        };
    }

} // namespace ml
