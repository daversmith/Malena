// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/RadioGroup.h>
#include <algorithm>

namespace ml
{
    RadioGroup::RadioGroup(const sf::Font& font_)
    {
        RadioGroupTheme::applyFrom(ThemeManager::get());
        this->font = &font_;
        _background.setFillColor(sf::Color::Transparent);
        _background.setOutlineThickness(0.f);
    }

    void RadioGroup::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        RadioGroupTheme::applyFrom(theme);
        for (auto* btn : _buttons) applyStylesToButton(*btn);
        layout();
    }

    void RadioGroup::applyStylesToButton(RadioButton& btn)
    {
        btn.applySettings(static_cast<const RadioButtonSettings&>(*this));
        btn.applyTheme(static_cast<const RadioButtonTheme&>(*this));
    }

    void RadioGroup::wire(RadioButton& btn)
    {
        btn.onClick([this, &btn]{ handleClick(btn); });
    }

    void RadioGroup::handleClick(RadioButton& clicked)
    {
        if (!clicked.isEnabled()) return;
        if (_selected == &clicked) return;
        if (_selected) _selected->deselect();
        _selected = &clicked;
        _selected->select();
        if (_onSelectionChanged)
            _onSelectionChanged(_selected->getLabel(), getSelectedIndex());
    }

    void RadioGroup::layout()
    {
        if (_buttons.empty()) return;

        const float startX = _position.x + padding;
        float y = _position.y + padding;

        for (auto* btn : _buttons)
        {
            btn->setPosition({startX, y});
            y += radius * 2.f + spacing;
        }

        if (showBackground)
        {
            float maxWidth = 0.f;
            for (auto* btn : _buttons)
                maxWidth = std::max(maxWidth, btn->getGlobalBounds().size.x);

            _background.setSize({maxWidth + padding * 2.f,
                                  (y - spacing) - _position.y + padding});
            _background.setPosition(_position);
            _background.setFillColor(bgColor);
            _background.setOutlineColor(bgOutlineColor);
            _background.setOutlineThickness(bgOutlineThickness);
            _background.setRadius(bgRadius);
        }
    }

    void RadioGroup::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (showBackground) target.draw(_background, states);
        for (const auto* btn : _buttons) target.draw(*btn, states);
    }

    void RadioGroup::addOption(const std::string& label)
    {
        auto btn = std::make_unique<RadioButton>(label, *font);
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

    std::size_t RadioGroup::optionCount() const { return _buttons.size(); }

    void RadioGroup::select(std::size_t i)
    { if (i < _buttons.size()) handleClick(*_buttons[i]); }

    void RadioGroup::selectFirst() { select(0); }

    std::string RadioGroup::getSelectedLabel() const
    { return _selected ? _selected->getLabel() : ""; }

    std::size_t RadioGroup::getSelectedIndex() const
    {
        if (!_selected) return std::string::npos;
        for (std::size_t i = 0; i < _buttons.size(); ++i)
            if (_buttons[i] == _selected) return i;
        return std::string::npos;
    }

    RadioButton* RadioGroup::getSelected() const { return _selected; }

    void RadioGroup::onSelectionChanged(
        std::function<void(const std::string&, std::size_t)> cb)
    { _onSelectionChanged = std::move(cb); }

    void RadioGroup::setOptionEnabled(std::size_t i, bool enabled)
    { if (i < _buttons.size()) _buttons[i]->setEnabled(enabled); }

    void RadioGroup::setPosition(const sf::Vector2f& pos)
    { _position = pos; layout(); }

    sf::Vector2f RadioGroup::getPosition() const { return _position; }

    sf::FloatRect RadioGroup::getGlobalBounds() const
    {
        if (showBackground) return _background.getGlobalBounds();
        if (_buttons.empty()) return sf::FloatRect{_position, {0.f, 0.f}};

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
