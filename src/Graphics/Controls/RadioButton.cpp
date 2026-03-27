// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/RadioButton.h>
#include <Malena/Engine/Events/Event.h>
#include <algorithm>
#include <cmath>

namespace ml
{
    // =========================================================================
    // RadioButton
    // =========================================================================

    RadioButton::RadioButton(const std::string& label, const sf::Font& font)
        : _label(font)
    {
        // Ring — transparent fill, colored outline
        _ring.setFillColor(sf::Color::Transparent);
        _ring.setOutlineThickness(2.f);

        // Dot — filled, hidden by default
        _dot.setFillColor(_dotColor);
        _dot.setOutlineThickness(0.f);

        // Label
        _label.setString(label);
        _label.setCharacterSize(16);
        _label.setFillColor(_labelColor);

        // Wire hover events for visual feedback
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

    // ── Layout ────────────────────────────────────────────────────────────────

	void RadioButton::layout()
    {
    	const sf::Vector2f pos = getPosition();

    	// Ring
    	_ring.setRadius(_radius);
    	_ring.setOrigin({_radius, _radius});
    	_ring.setPosition(pos + sf::Vector2f{_radius, _radius});

    	// Dot — centered inside ring
    	const float dotRadius = _radius * std::clamp(_dotScale, 0.1f, 0.9f); std::cout << "Dot radius: " << dotRadius;
    	_dot.setRadius(dotRadius);
    	_dot.setOrigin({dotRadius, dotRadius});
    	_dot.setPosition(_ring.getPosition());

    	// Label — use Align to place it to the right of the ring
    	Align::setRightOf(_ring, _label, _labelOffset);

    	// Then vertically center it against the ring
    	Align::centerVertically(_ring, _label);
    }

    void RadioButton::applyVisualState()
    {
        const bool disabled = checkFlag(Flag::DISABLED);
        const bool selected = checkFlag(Flag::SELECTED);

        if (disabled)
        {
            _ring.setOutlineColor(_ringDisabledColor);
            _dot.setFillColor(_dotDisabledColor);
            _label.setFillColor(_labelDisabledColor);
        }
        else if (selected)
        {
            _ring.setOutlineColor(_ringSelectedColor);
            _dot.setFillColor(_dotColor);
            _label.setFillColor(_labelColor);
        }
        else if (isState(State::HOVERED))
        {
            _ring.setOutlineColor(_ringHoverColor);
            _label.setFillColor(_labelColor);
        }
        else
        {
            _ring.setOutlineColor(_ringColor);
            _label.setFillColor(_labelColor);
        }
    }

    // ── Draw ──────────────────────────────────────────────────────────────────

    void RadioButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(_ring, states);

        if (checkFlag(Flag::SELECTED))
            target.draw(_dot, states);

        target.draw(_label, states);
    }

    // ── Selection ─────────────────────────────────────────────────────────────

    void RadioButton::select()
    {
        if (checkFlag(Flag::DISABLED))
            return;

        enableFlag(Flag::SELECTED);
        setState(State::SELECTED);
        applyVisualState();

        // Fire Selectable::select() to invoke onSelected callbacks
        Selectable::select();
    }

    void RadioButton::deselect()
    {
        disableFlag(Flag::SELECTED);
        setState(State::IDLE);
        applyVisualState();

        // Fire Selectable::deselect() to invoke onDeselected callbacks
        Selectable::deselect();
    }

    bool RadioButton::isSelected() const
    {
        return checkFlag(Flag::SELECTED);
    }

    // ── Enabled / disabled ────────────────────────────────────────────────────

    void RadioButton::setEnabled(bool enabled)
    {
        if (enabled)
        {
            disableFlag(Flag::DISABLED);
            setState(State::IDLE);
        }
        else
        {
            enableFlag(Flag::DISABLED);
            setState(State::DISABLED);
        }
        applyVisualState();
    }

    bool RadioButton::isEnabled() const
    {
        return !checkFlag(Flag::DISABLED);
    }

    // ── Layout setters ────────────────────────────────────────────────────────

    void RadioButton::setRadius(float radius)
    {
        _radius = radius;
        layout();
    }

    float RadioButton::getRadius() const
    {
        return _radius;
    }

    void RadioButton::setDotScale(float scale)
    {
        _dotScale = std::clamp(scale, 0.1f, 0.9f);
        layout();
    }

    float RadioButton::getDotScale() const
    {
        return _dotScale;
    }

    void RadioButton::setLabelOffset(float offset)
    {
        _labelOffset = offset;
        layout();
    }

    float RadioButton::getLabelOffset() const
    {
        return _labelOffset;
    }

    // ── Label ─────────────────────────────────────────────────────────────────

    void RadioButton::setLabel(const std::string& label)
    {
        _label.setString(label);
        layout();
    }

    std::string RadioButton::getLabel() const
    {
        return _label.getString();
    }

    void RadioButton::setCharacterSize(unsigned int size)
    {
        _label.setCharacterSize(size);
        layout();
    }

    unsigned int RadioButton::getCharacterSize() const
    {
        return _label.getCharacterSize();
    }

    void RadioButton::setFont(const sf::Font& font)
    {
        _label.setFont(font);
        layout();
    }

    // ── Colors ────────────────────────────────────────────────────────────────

    void RadioButton::setRingColor(const sf::Color& color)
    {
        _ringColor = color;
        applyVisualState();
    }

    void RadioButton::setRingHoverColor(const sf::Color& color)
    {
        _ringHoverColor = color;
        applyVisualState();
    }

    void RadioButton::setRingSelectedColor(const sf::Color& color)
    {
        _ringSelectedColor = color;
        applyVisualState();
    }

    void RadioButton::setDotColor(const sf::Color& color)
    {
        _dotColor = color;
        applyVisualState();
    }

    void RadioButton::setRingThickness(float thickness)
    {
        _ring.setOutlineThickness(thickness);
    }

    void RadioButton::setLabelColor(const sf::Color& color)
    {
        _labelColor = color;
        applyVisualState();
    }

    // ── Positionable overrides ────────────────────────────────────────────────

    void RadioButton::setPosition(const sf::Vector2f& position)
    {
        _position = position;
        layout();
    }

    sf::Vector2f RadioButton::getPosition() const
    {
        return _position;
    }

    sf::FloatRect RadioButton::getGlobalBounds() const
    {
        // Bounds span from ring left edge to label right edge
        const sf::FloatRect labelBounds = _label.getGlobalBounds();
        const float left   = _position.x;
        const float top    = _position.y;
        const float width  = labelBounds.position.x + labelBounds.size.x - left;
        const float height = _radius * 2.f;

        return sf::FloatRect{{left, top}, {width, height}};
    }

    // =========================================================================
    // RadioGroup
    // =========================================================================

    void RadioGroup::add(RadioButton& button)
    {
        _buttons.push_back(&button);

        button.onClick([this, &button]{
            handleClick(button);
        });
    }

    void RadioGroup::handleClick(RadioButton& clicked)
    {
        if (!clicked.isEnabled())
            return;

        if (_selected == &clicked)
            return;

        if (_selected)
            _selected->deselect();

        _selected = &clicked;
        _selected->select();

        if (_onSelectionChanged)
            _onSelectionChanged(*_selected);
    }

    void RadioGroup::select(std::size_t index)
    {
        if (index >= _buttons.size())
            return;

        handleClick(*_buttons[index]);
    }

    void RadioGroup::selectFirst()
    {
        select(0);
    }

    RadioButton* RadioGroup::getSelected() const
    {
        return _selected;
    }

    std::size_t RadioGroup::getSelectedIndex() const
    {
        if (!_selected)
            return std::string::npos;

        for (std::size_t i = 0; i < _buttons.size(); ++i)
            if (_buttons[i] == _selected)
                return i;

        return std::string::npos;
    }

    std::size_t RadioGroup::size() const
    {
        return _buttons.size();
    }

    void RadioGroup::onSelectionChanged(std::function<void(RadioButton&)> callback)
    {
        _onSelectionChanged = std::move(callback);
    }

} // namespace ml
