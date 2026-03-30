// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/Checkbox.h>
#include <Malena/Utilities/Align.h>
#include <algorithm>
#include <cmath>

namespace ml
{
    // ── Constructor ───────────────────────────────────────────────────────────

    Checkbox::Checkbox(const std::string& label, const sf::Font& font)
        : _label(font),
          _checkmark(sf::PrimitiveType::LineStrip, 3)
    {
        _box.setOutlineThickness(1.5f);
        _box.setOutlineColor(_outlineColor);
        _box.setFillColor(_boxColor);

        _label.setString(label);
        _label.setCharacterSize(16);
        _label.setFillColor(_labelColor);

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
			if (!checkFlag(Flag::DISABLED))
				toggle();
		});

        setState(State::IDLE);
        layout();
        applyVisualState();
    }

    // ── Layout ────────────────────────────────────────────────────────────────

    void Checkbox::layout()
    {
        _box.setSize({_boxSize, _boxSize});
        _box.setPosition(_position);

        buildCheckmark();

        // Label — vertically centered beside box
        Align::setRightOf(_box, _label, _labelOffset);
        Align::centerVertically(_box, _label);
    }

    void Checkbox::buildCheckmark()
    {
        // Checkmark as a 3-point line strip — forms a tick shape
        // Points are relative to box position, inset by ~25%
        const float inset = _boxSize * 0.2f;
        const float x = _position.x;
        const float y = _position.y;

        _checkmark[0].position = { x + inset,              y + _boxSize * 0.55f };
        _checkmark[1].position = { x + _boxSize * 0.42f,   y + _boxSize - inset };
        _checkmark[2].position = { x + _boxSize - inset,   y + inset            };

        _checkmark[0].color = _checkColor;
        _checkmark[1].color = _checkColor;
        _checkmark[2].color = _checkColor;
    }

    void Checkbox::applyVisualState()
    {
        const bool disabled = checkFlag(Flag::DISABLED);
        const bool checked  = checkFlag(Flag::CHECKED);

        if (disabled)
        {
            _box.setFillColor(_boxDisabledColor);
            _box.setOutlineColor(_boxDisabledColor);
            _label.setFillColor(_labelDisabledColor);
            for (std::size_t i = 0; i < _checkmark.getVertexCount(); ++i)
                _checkmark[i].color = _checkDisabledColor;
        }
        else if (checked)
        {
            _box.setFillColor(_boxCheckedColor);
            _box.setOutlineColor(_boxCheckedColor);
            _label.setFillColor(_labelColor);
            for (std::size_t i = 0; i < _checkmark.getVertexCount(); ++i)
                _checkmark[i].color = _checkColor;
        }
        else if (isState(State::HOVERED))
        {
            _box.setFillColor(_boxHoverColor);
            _box.setOutlineColor(_outlineColor);
            _label.setFillColor(_labelColor);
        }
        else
        {
            _box.setFillColor(_boxColor);
            _box.setOutlineColor(_outlineColor);
            _label.setFillColor(_labelColor);
        }
    }

    // ── Draw ──────────────────────────────────────────────────────────────────

    void Checkbox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(_box, states);

        if (checkFlag(Flag::CHECKED))
            target.draw(_checkmark, states);

        target.draw(_label, states);
    }

    // ── Checked state ─────────────────────────────────────────────────────────

    void Checkbox::check()
    {
        if (checkFlag(Flag::DISABLED))
            return;

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
        if (checkFlag(Flag::CHECKED))
            uncheck();
        else
            check();
    }

    bool Checkbox::isChecked() const
    {
        return checkFlag(Flag::CHECKED);
    }

    // ── Enabled / disabled ────────────────────────────────────────────────────

    void Checkbox::setEnabled(bool enabled)
    {
        if (enabled)
        {
            disableFlag(Flag::DISABLED);
            setState(isChecked() ? State::CHECKED : State::IDLE);
        }
        else
        {
            enableFlag(Flag::DISABLED);
            setState(State::DISABLED);
        }
        applyVisualState();
    }

    bool Checkbox::isEnabled() const
    {
        return !checkFlag(Flag::DISABLED);
    }

    // ── Layout setters ────────────────────────────────────────────────────────

    void Checkbox::setBoxSize(float size)
    {
        _boxSize = size;
        layout();
    }

    float Checkbox::getBoxSize() const
    {
        return _boxSize;
    }

    void Checkbox::setBoxRadius(float radius)
    {
        _boxRadius = radius;
        // sf::RectangleShape has no built-in radius — would need RoundedRectangle
        // for rounded checkbox corners. Stored for future use.
        layout();
    }

    float Checkbox::getBoxRadius() const
    {
        return _boxRadius;
    }

    void Checkbox::setLabelOffset(float offset)
    {
        _labelOffset = offset;
        layout();
    }

    float Checkbox::getLabelOffset() const
    {
        return _labelOffset;
    }

    // ── Label ─────────────────────────────────────────────────────────────────

    void Checkbox::setLabel(const std::string& label)
    {
        _label.setString(label);
        layout();
    }

    std::string Checkbox::getLabel() const
    {
        return _label.getString();
    }

    void Checkbox::setCharacterSize(unsigned int size)
    {
        _label.setCharacterSize(size);
        layout();
    }

    unsigned int Checkbox::getCharacterSize() const
    {
        return _label.getCharacterSize();
    }

    void Checkbox::setFont(const sf::Font& font)
    {
        _label.setFont(font);
        layout();
    }

    // ── Colors ────────────────────────────────────────────────────────────────

    void Checkbox::setBoxColor(const sf::Color& color)
    {
        _boxColor = color;
        applyVisualState();
    }

    void Checkbox::setBoxHoverColor(const sf::Color& color)
    {
        _boxHoverColor = color;
        applyVisualState();
    }

    void Checkbox::setBoxCheckedColor(const sf::Color& color)
    {
        _boxCheckedColor = color;
        applyVisualState();
    }

    void Checkbox::setOutlineColor(const sf::Color& color)
    {
        _outlineColor = color;
        applyVisualState();
    }

    void Checkbox::setCheckColor(const sf::Color& color)
    {
        _checkColor = color;
        buildCheckmark();
    }

    void Checkbox::setLabelColor(const sf::Color& color)
    {
        _labelColor = color;
        applyVisualState();
    }

    // ── Positionable overrides ────────────────────────────────────────────────

    void Checkbox::setPosition(const sf::Vector2f& position)
    {
        _position = position;
        layout();
    }

    sf::Vector2f Checkbox::getPosition() const
    {
        return _position;
    }

    sf::FloatRect Checkbox::getGlobalBounds() const
    {
        const sf::FloatRect labelBounds = _label.getGlobalBounds();
        const float left   = _position.x;
        const float top    = _position.y;
        const float width  = labelBounds.position.x + labelBounds.size.x - left;
        const float height = _boxSize;

        return sf::FloatRect{{left, top}, {width, height}};
    }

} // namespace ml
