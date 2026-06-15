// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#include <Malena/Graphics/Controls/ColorButton.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <Malena/Engine/App/AppManager.h>   // exclusive-owner (modal) input gating

#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace ml
{
    ColorButton::ColorButton()
    {
        this->setFillColor(sf::Color::Transparent);

        onUpdate([this] {
            if (_colorProvider) _color = _colorProvider();   // live "color at cursor"

            // Skip when hidden/disabled OR when a modal owns input (don't click through).
            if (checkFlag(ml::Flag::HIDDEN) || !checkFlag(ml::Flag::ENABLED) ||
                !AppManager::isUnderExclusiveOwner(this))
            {
                _hover = false;
                _prevMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
                return;
            }

            const auto& win = WindowManager::getWindow();
            const sf::Vector2f wp = win.mapPixelToCoords(sf::Mouse::getPosition(win));
            const bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

            _hover = getGlobalBounds().contains(wp);
            if (down && !_prevMouseDown && _hover && _onClick) _onClick();
            _prevMouseDown = down;
        });
    }

    void ColorButton::setColor(const sf::Color& c) { _color = c; }
    void ColorButton::onClick(std::function<void()> cb) { _onClick = std::move(cb); }
    void ColorButton::setColorProvider(std::function<sf::Color()> provider)
    { _colorProvider = std::move(provider); }

    void ColorButton::setSize(const sf::Vector2f& size)
    { _size = size; Graphic<sf::RectangleShape>::setSize(size); }

    void ColorButton::setPosition(const sf::Vector2f& pos)
    { _pos = pos; Graphic<sf::RectangleShape>::setPosition(pos); }

    sf::Vector2f  ColorButton::getPosition()     const { return _pos; }
    sf::FloatRect ColorButton::getGlobalBounds() const { return { _pos, _size }; }

    void ColorButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        sf::RectangleShape box(_size);
        box.setPosition(_pos);
        box.setFillColor(_color);
        box.setOutlineThickness(_hover ? 2.f : 1.f);
        box.setOutlineColor(_hover ? sf::Color(70, 110, 200) : sf::Color(150, 150, 160));
        target.draw(box, states);
    }

} // namespace ml
