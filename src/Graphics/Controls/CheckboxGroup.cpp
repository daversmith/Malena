// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/CheckboxGroup.h>
#include <algorithm>

namespace ml
{
    CheckboxGroup::CheckboxGroup(const sf::Font& font_)
    {
        CheckboxGroupTheme::applyFrom(ThemeManager::get());
        this->font = &font_;
        _background.setFillColor(sf::Color::Transparent);
        _background.setOutlineThickness(0.f);
    }

    void CheckboxGroup::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        CheckboxGroupTheme::applyFrom(theme);
        for (auto* cb : _checkboxes) applyStylesToCheckbox(*cb);
        layout();
    }

    void CheckboxGroup::applyStylesToCheckbox(Checkbox& cb)
    {
        cb.applySettings(static_cast<const CheckboxSettings&>(*this));
        cb.applyTheme(static_cast<const CheckboxTheme&>(*this));
    }

    void CheckboxGroup::wire(Checkbox& cb)
    {
        cb.onSelected([this]{
            if (_onSelectionChanged) _onSelectionChanged(getCheckedLabels());
        });
        cb.onDeselected([this]{
            if (_onSelectionChanged) _onSelectionChanged(getCheckedLabels());
        });
    }

    void CheckboxGroup::layout()
    {
        if (_checkboxes.empty()) return;

        const float startX = _position.x + padding;
        float y = _position.y + padding;

        for (auto* cb : _checkboxes)
        {
            cb->setPosition({startX, y});
            y += boxSize + spacing;
        }

        if (showBackground)
        {
            float maxWidth = 0.f;
            for (auto* cb : _checkboxes)
                maxWidth = std::max(maxWidth, cb->getGlobalBounds().size.x);

            _background.setSize({maxWidth + padding * 2.f,
                                  (y - spacing) - _position.y + padding});
            _background.setPosition(_position);
            _background.setFillColor(bgColor);
            _background.setOutlineColor(bgOutlineColor);
            _background.setOutlineThickness(bgOutlineThickness);
            _background.setRadius(bgRadius);
        }
    }

    void CheckboxGroup::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (showBackground) target.draw(_background, states);
        for (const auto* cb : _checkboxes) target.draw(*cb, states);
    }

    void CheckboxGroup::addOption(const std::string& label, bool checked)
    {
        auto cb = std::make_unique<Checkbox>(label, *font);
        applyStylesToCheckbox(*cb);
        wire(*cb);
        if (checked) cb->check();
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

    std::size_t CheckboxGroup::optionCount() const { return _checkboxes.size(); }

    bool CheckboxGroup::isChecked(std::size_t i) const
    {
        return i < _checkboxes.size() && _checkboxes[i]->isChecked();
    }

    std::vector<std::string> CheckboxGroup::getCheckedLabels() const
    {
        std::vector<std::string> result;
        for (const auto* cb : _checkboxes)
            if (cb->isChecked()) result.push_back(cb->getLabel());
        return result;
    }

    std::vector<std::size_t> CheckboxGroup::getCheckedIndices() const
    {
        std::vector<std::size_t> result;
        for (std::size_t i = 0; i < _checkboxes.size(); ++i)
            if (_checkboxes[i]->isChecked()) result.push_back(i);
        return result;
    }

    void CheckboxGroup::check(std::size_t i)
    { if (i < _checkboxes.size()) _checkboxes[i]->check(); }

    void CheckboxGroup::uncheck(std::size_t i)
    { if (i < _checkboxes.size()) _checkboxes[i]->uncheck(); }

    void CheckboxGroup::checkAll()   { for (auto* cb : _checkboxes) cb->check(); }
    void CheckboxGroup::uncheckAll() { for (auto* cb : _checkboxes) cb->uncheck(); }

    void CheckboxGroup::onSelectionChanged(
        std::function<void(const std::vector<std::string>&)> cb)
    { _onSelectionChanged = std::move(cb); }

    void CheckboxGroup::setOptionEnabled(std::size_t i, bool enabled)
    { if (i < _checkboxes.size()) _checkboxes[i]->setEnabled(enabled); }

    void CheckboxGroup::setPosition(const sf::Vector2f& pos)
    { _position = pos; layout(); }

    sf::Vector2f CheckboxGroup::getPosition() const { return _position; }

    sf::FloatRect CheckboxGroup::getGlobalBounds() const
    {
        if (showBackground) return _background.getGlobalBounds();
        if (_checkboxes.empty()) return sf::FloatRect{_position, {0.f, 0.f}};

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
