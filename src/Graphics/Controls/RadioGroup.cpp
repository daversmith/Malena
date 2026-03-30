// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/RadioGroup.h>
#include <algorithm>

namespace ml
{
    // ── Constructor ───────────────────────────────────────────────────────────

    RadioGroup::RadioGroup(const sf::Font& font)
        : _font(&font)
    {
        _background.setFillColor(sf::Color::Transparent);
        _background.setOutlineThickness(0.f);
    }

    // ── Internal helpers ──────────────────────────────────────────────────────

    void RadioGroup::applyStylesToButton(RadioButton& btn)
    {
        btn.setRadius(_radius);
        btn.setDotScale(_dotScale);
        btn.setLabelOffset(_labelOffset);
        btn.setRingThickness(_ringThickness);
        btn.setCharacterSize(_charSize);
        btn.setRingColor(_ringColor);
        btn.setRingHoverColor(_ringHoverColor);
        btn.setRingSelectedColor(_ringSelColor);
        btn.setDotColor(_dotColor);
        btn.setLabelColor(_labelColor);
        btn.setFont(*_font);
    }

    void RadioGroup::wire(RadioButton& btn)
    {
        btn.onClick([this, &btn]{
            handleClick(btn);
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
        {
            const std::size_t index = getSelectedIndex();
            _onSelectionChanged(_selected->getLabel(), index);
        }
    }

    void RadioGroup::layout()
    {
        if (_buttons.empty())
            return;

        const float startX = _position.x + _padding;
        const float startY = _position.y + _padding;

        float y = startY;
        for (auto* btn : _buttons)
        {
            btn->setPosition({startX, y});
            y += _radius * 2.f + _spacing;
        }

        // Resize background to fit all buttons
        if (_showBackground)
        {
            // Find widest button for background width
            float maxWidth = 0.f;
            for (auto* btn : _buttons)
            {
                const float w = btn->getGlobalBounds().size.x;
                if (w > maxWidth) maxWidth = w;
            }

            const float bgWidth  = maxWidth + _padding * 2.f;
            const float bgHeight = (y - _spacing) - _position.y + _padding;
            _background.setSize({bgWidth, bgHeight});
            _background.setPosition(_position);
        }
    }

    // ── Draw ──────────────────────────────────────────────────────────────────

    void RadioGroup::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (_showBackground)
            target.draw(_background, states);

        for (const auto* btn : _buttons)
            target.draw(*btn, states);
    }

    // ── Adding options ────────────────────────────────────────────────────────

    void RadioGroup::addOption(const std::string& label)
    {
        auto btn = std::make_unique<RadioButton>(label, *_font);
        applyStylesToButton(*btn);
        wire(*btn);
        _buttons.push_back(btn.get());
        _owned.push_back(std::move(btn));
        layout();
    }

    void RadioGroup::add(RadioButton& button)
    {
        applyStylesToButton(button);
        wire(button);
        _buttons.push_back(&button);
        layout();
    }

    void RadioGroup::clearOptions()
    {
        _selected = nullptr;
        _buttons.clear();
        _owned.clear();
    }

    std::size_t RadioGroup::optionCount() const
    {
        return _buttons.size();
    }

    // ── Selection ─────────────────────────────────────────────────────────────

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

    std::string RadioGroup::getSelectedLabel() const
    {
        return _selected ? _selected->getLabel() : "";
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

    RadioButton* RadioGroup::getSelected() const
    {
        return _selected;
    }

    void RadioGroup::onSelectionChanged(std::function<void(const std::string&, std::size_t)> callback)
    {
        _onSelectionChanged = std::move(callback);
    }

    // ── Enabled / disabled ────────────────────────────────────────────────────

    void RadioGroup::setOptionEnabled(std::size_t index, bool enabled)
    {
        if (index >= _buttons.size())
            return;

        _buttons[index]->setEnabled(enabled);
    }

    // ── Background ────────────────────────────────────────────────────────────

    void RadioGroup::showBackground(bool show)
    {
        _showBackground = show;
        enableFlag(Flag::SHOW_BACKGROUND);
        layout();
    }

    void RadioGroup::setBackgroundColor(const sf::Color& color)
    {
        _background.setFillColor(color);
    }

    void RadioGroup::setBackgroundOutlineColor(const sf::Color& color)
    {
        _background.setOutlineColor(color);
    }

    void RadioGroup::setBackgroundOutlineThickness(float thickness)
    {
        _background.setOutlineThickness(thickness);
    }

    void RadioGroup::setBackgroundRadius(float radius)
    {
        _background.setRadius(radius);
    }

    void RadioGroup::setPadding(float padding)
    {
        _padding = padding;
        layout();
    }

    float RadioGroup::getPadding() const
    {
        return _padding;
    }

    // ── Layout ────────────────────────────────────────────────────────────────

    void RadioGroup::setSpacing(float spacing)
    {
        _spacing = spacing;
        layout();
    }

    float RadioGroup::getSpacing() const
    {
        return _spacing;
    }

    // ── Visual properties ─────────────────────────────────────────────────────

    void RadioGroup::setRadius(float radius)
    {
        _radius = radius;
        for (auto* btn : _buttons) btn->setRadius(radius);
        layout();
    }

    void RadioGroup::setDotScale(float scale)
    {
        _dotScale = scale;
        for (auto* btn : _buttons) btn->setDotScale(scale);
    }

    void RadioGroup::setLabelOffset(float offset)
    {
        _labelOffset = offset;
        for (auto* btn : _buttons) btn->setLabelOffset(offset);
        layout();
    }

    void RadioGroup::setRingThickness(float thickness)
    {
        _ringThickness = thickness;
        for (auto* btn : _buttons) btn->setRingThickness(thickness);
    }

    void RadioGroup::setCharacterSize(unsigned int size)
    {
        _charSize = size;
        for (auto* btn : _buttons) btn->setCharacterSize(size);
        layout();
    }

    void RadioGroup::setRingColor(const sf::Color& color)
    {
        _ringColor = color;
        for (auto* btn : _buttons) btn->setRingColor(color);
    }

    void RadioGroup::setRingHoverColor(const sf::Color& color)
    {
        _ringHoverColor = color;
        for (auto* btn : _buttons) btn->setRingHoverColor(color);
    }

    void RadioGroup::setRingSelectedColor(const sf::Color& color)
    {
        _ringSelColor = color;
        for (auto* btn : _buttons) btn->setRingSelectedColor(color);
    }

    void RadioGroup::setDotColor(const sf::Color& color)
    {
        _dotColor = color;
        for (auto* btn : _buttons) btn->setDotColor(color);
    }

    void RadioGroup::setLabelColor(const sf::Color& color)
    {
        _labelColor = color;
        for (auto* btn : _buttons) btn->setLabelColor(color);
    }

    void RadioGroup::setFont(const sf::Font& font)
    {
        _font = &font;
        for (auto* btn : _buttons) btn->setFont(font);
        layout();
    }

    // ── Positionable overrides ────────────────────────────────────────────────

    void RadioGroup::setPosition(const sf::Vector2f& position)
    {
        _position = position;
        layout();
    }

    sf::Vector2f RadioGroup::getPosition() const
    {
        return _position;
    }

    sf::FloatRect RadioGroup::getGlobalBounds() const
    {
        if (_showBackground)
            return _background.getGlobalBounds();

        if (_buttons.empty())
            return sf::FloatRect{_position, {0.f, 0.f}};

        // Union of all button bounds
        sf::FloatRect bounds = _buttons.front()->getGlobalBounds();
        for (const auto* btn : _buttons)
        {
            const sf::FloatRect b = btn->getGlobalBounds();
            const float right  = std::max(bounds.position.x + bounds.size.x,
                                          b.position.x + b.size.x);
            const float bottom = std::max(bounds.position.y + bounds.size.y,
                                          b.position.y + b.size.y);
            bounds.position.x = std::min(bounds.position.x, b.position.x);
            bounds.position.y = std::min(bounds.position.y, b.position.y);
            bounds.size.x     = right  - bounds.position.x;
            bounds.size.y     = bottom - bounds.position.y;
        }
        return bounds;
    }

} // namespace ml
