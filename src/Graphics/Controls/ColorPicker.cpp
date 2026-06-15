// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#include <Malena/Graphics/Controls/ColorPicker.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <Malena/Engine/App/AppManager.h>   // exclusive-owner (modal) input gating

#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>

namespace ml
{
    ColorPicker::ColorPicker()
    {
        _palette = {
            sf::Color( 30,  30,  30), sf::Color( 90,  90,  90), sf::Color(150, 150, 150),
            sf::Color(210, 210, 210), sf::Color(255, 255, 255), sf::Color(140,  40,  40),
            sf::Color(200,  60,  60), sf::Color(220, 120,  40), sf::Color(220, 190,  50),
            sf::Color( 90, 160,  70), sf::Color( 60, 160,  90), sf::Color( 40, 150, 150),
            sf::Color( 70, 110, 200), sf::Color( 40,  70, 150), sf::Color(120,  70, 180),
            sf::Color(180,  70, 150),
        };
        this->setFillColor(sf::Color::White);

        recomputeSize();

        onUpdate([this] {
            if (checkFlag(ml::Flag::HIDDEN) || !checkFlag(ml::Flag::ENABLED) ||
                !AppManager::isUnderExclusiveOwner(this))
            {
                _hoverIndex     = -1;
                _prevMouseDown  = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
                _prevRightDown  = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
                return;
            }

            const auto& win = WindowManager::getWindow();
            const sf::Vector2f wp =
                win.mapPixelToCoords(sf::Mouse::getPosition(win));
            const bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

            _hoverIndex = -1;
            for (std::size_t i = 0; i < _palette.size(); ++i)
                if (cellRect(i).contains(wp)) { _hoverIndex = static_cast<int>(i); break; }

            if (down && !_prevMouseDown && _hoverIndex >= 0 && _onColorSelected)
                _onColorSelected(_palette[static_cast<std::size_t>(_hoverIndex)]);

            // Right-click on a swatch → notify (e.g. to show a context menu).
            const bool rdown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
            if (rdown && !_prevRightDown && _hoverIndex >= 0 && _onSwatchRightClick)
                _onSwatchRightClick(static_cast<std::size_t>(_hoverIndex), wp);

            _prevMouseDown = down;
            _prevRightDown = rdown;
        });
    }

    void ColorPicker::setPalette(const std::vector<sf::Color>& colors)
    { _palette = colors; recomputeSize(); }

    void ColorPicker::addColor(const sf::Color& c)
    {
        _palette.push_back(c);
        if (_maxColors > 0)
            while (_palette.size() > static_cast<std::size_t>(_maxColors))
                _palette.erase(_palette.begin());   // drop oldest
        recomputeSize();
    }

    void ColorPicker::removeColor(std::size_t index)
    {
        if (index < _palette.size())
        {
            const sf::Color removed = _palette[index];
            _palette.erase(_palette.begin() + static_cast<long>(index));
            recomputeSize();
            if (_onColorRemoved) _onColorRemoved(removed);
        }
    }

    void ColorPicker::setMaxColors(int max) { _maxColors = max; }

    void ColorPicker::onColorRemoved(std::function<void(const sf::Color&)> cb)
    { _onColorRemoved = std::move(cb); }

    void ColorPicker::onSwatchRightClick(std::function<void(std::size_t, const sf::Vector2f&)> cb)
    { _onSwatchRightClick = std::move(cb); }

    void ColorPicker::setColumns(int columns)
    { _cols = std::max(1, columns); recomputeSize(); }

    void ColorPicker::setSwatchSize(float px) { _swatch = px; recomputeSize(); }
    void ColorPicker::setSwatchGap(float px)  { _gap    = px; recomputeSize(); }

    void ColorPicker::onColorSelected(std::function<void(const sf::Color&)> cb)
    { _onColorSelected = std::move(cb); }

    void ColorPicker::recomputeSize()
    {
        const int n    = static_cast<int>(_palette.size());
        const int cols  = std::max(1, std::min(_cols, n == 0 ? 1 : n));
        const int rows  = (n + cols - 1) / std::max(1, cols);
        const float w = _pad * 2.f + cols * (_swatch + _gap) - _gap;
        const float h = _pad * 2.f + std::max(1, rows) * (_swatch + _gap) - _gap;
        this->setSize({ w, h });
    }

    sf::FloatRect ColorPicker::cellRect(std::size_t index) const
    {
        const int col = static_cast<int>(index) % _cols;
        const int row = static_cast<int>(index) / _cols;
        const float x = _pos.x + _pad + col * (_swatch + _gap);
        const float y = _pos.y + _pad + row * (_swatch + _gap);
        return { { x, y }, { _swatch, _swatch } };
    }

    void ColorPicker::setPosition(const sf::Vector2f& pos)
    {
        _pos = pos;
        Graphic<sf::RectangleShape>::setPosition(pos);
    }

    sf::Vector2f  ColorPicker::getPosition()     const { return _pos; }
    sf::FloatRect ColorPicker::getGlobalBounds() const { return { _pos, getSize() }; }

    void ColorPicker::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Background card.
        sf::RectangleShape bg(getSize());
        bg.setPosition(_pos);
        bg.setFillColor(sf::Color::White);
        bg.setOutlineColor(sf::Color(210, 212, 218));
        bg.setOutlineThickness(1.f);
        target.draw(bg, states);

        // Swatches.
        for (std::size_t i = 0; i < _palette.size(); ++i)
        {
            const sf::FloatRect r = cellRect(i);
            sf::RectangleShape cell({ r.size.x, r.size.y });
            cell.setPosition(r.position);
            cell.setFillColor(_palette[i]);
            const bool hovered = (static_cast<int>(i) == _hoverIndex);
            cell.setOutlineColor(hovered ? sf::Color(70, 110, 200)
                                         : sf::Color(200, 202, 208));
            cell.setOutlineThickness(hovered ? 2.f : 1.f);
            target.draw(cell, states);
        }
    }

} // namespace ml
