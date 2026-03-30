// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/CheckboxGroup.h>
#include <algorithm>

namespace ml
{
    // ── Constructor ───────────────────────────────────────────────────────────

    CheckboxGroup::CheckboxGroup(const sf::Font& font)
        : _font(&font)
    {
        _background.setFillColor(sf::Color::Transparent);
        _background.setOutlineThickness(0.f);
    }

    // ── Internal helpers ──────────────────────────────────────────────────────

    void CheckboxGroup::applyStylesToCheckbox(Checkbox& cb)
    {
        cb.setBoxSize(_boxSize);
        cb.setBoxRadius(_boxRadius);
        cb.setLabelOffset(_labelOffset);
        cb.setCharacterSize(_charSize);
        cb.setBoxColor(_boxColor);
        cb.setBoxHoverColor(_boxHoverColor);
        cb.setBoxCheckedColor(_boxCheckedColor);
        cb.setCheckColor(_checkColor);
        cb.setOutlineColor(_outlineColor);
        cb.setLabelColor(_labelColor);
        cb.setFont(*_font);
    }

    void CheckboxGroup::wire(Checkbox& cb)
    {
        cb.onSelected([this]{
            if (_onSelectionChanged)
                _onSelectionChanged(getCheckedLabels());
        });

        cb.onDeselected([this]{
            if (_onSelectionChanged)
                _onSelectionChanged(getCheckedLabels());
        });
    }

    void CheckboxGroup::layout()
    {
        if (_checkboxes.empty())
            return;

        const float startX = _position.x + _padding;
        float y = _position.y + _padding;

        for (auto* cb : _checkboxes)
        {
            cb->setPosition({startX, y});
            y += _boxSize + _spacing;
        }

        if (_showBackground)
        {
            float maxWidth = 0.f;
            for (auto* cb : _checkboxes)
            {
                const float w = cb->getGlobalBounds().size.x;
                if (w > maxWidth) maxWidth = w;
            }

            const float bgWidth  = maxWidth + _padding * 2.f;
            const float bgHeight = (y - _spacing) - _position.y + _padding;
            _background.setSize({bgWidth, bgHeight});
            _background.setPosition(_position);
        }
    }

    // ── Draw ──────────────────────────────────────────────────────────────────

    void CheckboxGroup::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (_showBackground)
            target.draw(_background, states);

        for (const auto* cb : _checkboxes)
            target.draw(*cb, states);
    }

    // ── Adding options ────────────────────────────────────────────────────────

    void CheckboxGroup::addOption(const std::string& label, bool checked)
    {
        auto cb = std::make_unique<Checkbox>(label, *_font);
        applyStylesToCheckbox(*cb);
        wire(*cb);

        if (checked)
            cb->check();

        _checkboxes.push_back(cb.get());
        _owned.push_back(std::move(cb));
        layout();
    }

    void CheckboxGroup::add(Checkbox& checkbox)
    {
        applyStylesToCheckbox(checkbox);
        wire(checkbox);
        _checkboxes.push_back(&checkbox);
        layout();
    }

    void CheckboxGroup::clearOptions()
    {
        _checkboxes.clear();
        _owned.clear();
    }

    std::size_t CheckboxGroup::optionCount() const
    {
        return _checkboxes.size();
    }

    // ── Querying state ────────────────────────────────────────────────────────

    bool CheckboxGroup::isChecked(std::size_t index) const
    {
        if (index >= _checkboxes.size())
            return false;

        return _checkboxes[index]->isChecked();
    }

    std::vector<std::string> CheckboxGroup::getCheckedLabels() const
    {
        std::vector<std::string> result;
        for (const auto* cb : _checkboxes)
            if (cb->isChecked())
                result.push_back(cb->getLabel());
        return result;
    }

    std::vector<std::size_t> CheckboxGroup::getCheckedIndices() const
    {
        std::vector<std::size_t> result;
        for (std::size_t i = 0; i < _checkboxes.size(); ++i)
            if (_checkboxes[i]->isChecked())
                result.push_back(i);
        return result;
    }

    // ── Programmatic control ──────────────────────────────────────────────────

    void CheckboxGroup::check(std::size_t index)
    {
        if (index >= _checkboxes.size())
            return;

        _checkboxes[index]->check();
    }

    void CheckboxGroup::uncheck(std::size_t index)
    {
        if (index >= _checkboxes.size())
            return;

        _checkboxes[index]->uncheck();
    }

    void CheckboxGroup::checkAll()
    {
        for (auto* cb : _checkboxes)
            cb->check();
    }

    void CheckboxGroup::uncheckAll()
    {
        for (auto* cb : _checkboxes)
            cb->uncheck();
    }

    // ── Callback ─────────────────────────────────────────────────────────────

    void CheckboxGroup::onSelectionChanged(
        std::function<void(const std::vector<std::string>&)> callback)
    {
        _onSelectionChanged = std::move(callback);
    }

    // ── Enabled / disabled ────────────────────────────────────────────────────

    void CheckboxGroup::setOptionEnabled(std::size_t index, bool enabled)
    {
        if (index >= _checkboxes.size())
            return;

        _checkboxes[index]->setEnabled(enabled);
    }

    // ── Background ────────────────────────────────────────────────────────────

    void CheckboxGroup::showBackground(bool show)
    {
        _showBackground = show;
        enableFlag(Flag::SHOW_BACKGROUND);
        layout();
    }

    void CheckboxGroup::setBackgroundColor(const sf::Color& color)
    {
        _background.setFillColor(color);
    }

    void CheckboxGroup::setBackgroundOutlineColor(const sf::Color& color)
    {
        _background.setOutlineColor(color);
    }

    void CheckboxGroup::setBackgroundOutlineThickness(float thickness)
    {
        _background.setOutlineThickness(thickness);
    }

    void CheckboxGroup::setBackgroundRadius(float radius)
    {
        _background.setRadius(radius);
    }

    void CheckboxGroup::setPadding(float padding)
    {
        _padding = padding;
        layout();
    }

    float CheckboxGroup::getPadding() const
    {
        return _padding;
    }

    // ── Layout ────────────────────────────────────────────────────────────────

    void CheckboxGroup::setSpacing(float spacing)
    {
        _spacing = spacing;
        layout();
    }

    float CheckboxGroup::getSpacing() const
    {
        return _spacing;
    }

    // ── Visual properties ─────────────────────────────────────────────────────

    void CheckboxGroup::setBoxSize(float size)
    {
        _boxSize = size;
        for (auto* cb : _checkboxes) cb->setBoxSize(size);
        layout();
    }

    void CheckboxGroup::setBoxRadius(float radius)
    {
        _boxRadius = radius;
        for (auto* cb : _checkboxes) cb->setBoxRadius(radius);
    }

    void CheckboxGroup::setLabelOffset(float offset)
    {
        _labelOffset = offset;
        for (auto* cb : _checkboxes) cb->setLabelOffset(offset);
        layout();
    }

    void CheckboxGroup::setCharacterSize(unsigned int size)
    {
        _charSize = size;
        for (auto* cb : _checkboxes) cb->setCharacterSize(size);
        layout();
    }

    void CheckboxGroup::setBoxColor(const sf::Color& color)
    {
        _boxColor = color;
        for (auto* cb : _checkboxes) cb->setBoxColor(color);
    }

    void CheckboxGroup::setBoxHoverColor(const sf::Color& color)
    {
        _boxHoverColor = color;
        for (auto* cb : _checkboxes) cb->setBoxHoverColor(color);
    }

    void CheckboxGroup::setBoxCheckedColor(const sf::Color& color)
    {
        _boxCheckedColor = color;
        for (auto* cb : _checkboxes) cb->setBoxCheckedColor(color);
    }

    void CheckboxGroup::setCheckColor(const sf::Color& color)
    {
        _checkColor = color;
        for (auto* cb : _checkboxes) cb->setCheckColor(color);
    }

    void CheckboxGroup::setOutlineColor(const sf::Color& color)
    {
        _outlineColor = color;
        for (auto* cb : _checkboxes) cb->setOutlineColor(color);
    }

    void CheckboxGroup::setLabelColor(const sf::Color& color)
    {
        _labelColor = color;
        for (auto* cb : _checkboxes) cb->setLabelColor(color);
    }

    void CheckboxGroup::setFont(const sf::Font& font)
    {
        _font = &font;
        for (auto* cb : _checkboxes) cb->setFont(font);
        layout();
    }

    // ── Positionable overrides ────────────────────────────────────────────────

    void CheckboxGroup::setPosition(const sf::Vector2f& position)
    {
        _position = position;
        layout();
    }

    sf::Vector2f CheckboxGroup::getPosition() const
    {
        return _position;
    }

    sf::FloatRect CheckboxGroup::getGlobalBounds() const
    {
        if (_showBackground)
            return _background.getGlobalBounds();

        if (_checkboxes.empty())
            return sf::FloatRect{_position, {0.f, 0.f}};

        sf::FloatRect bounds = _checkboxes.front()->getGlobalBounds();
        for (const auto* cb : _checkboxes)
        {
            const sf::FloatRect b = cb->getGlobalBounds();
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
