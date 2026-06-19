// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#include <Malena/Graphics/Controls/ContextMenu.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <Malena/Engine/App/AppManager.h>   // exclusive-owner (modal) input gating
#include <Malena/Resources/FontManager.h>

#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

#include <algorithm>

namespace ml
{
    ContextMenu::ContextMenu()
    {
        this->setFillColor(sf::Color::Transparent);
        hide();

        onUpdate([this] {
            if (!_open || !AppManager::isUnderExclusiveOwner(this))
            { _prevMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left); return; }

            const auto& win = WindowManager::getWindow();
            const sf::Vector2f wp = win.mapPixelToCoords(sf::Mouse::getPosition(win));
            const bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

            _hover = -1;
            for (std::size_t i = 0; i < _items.size(); ++i)
                if (itemRect(i).contains(wp)) { _hover = static_cast<int>(i); break; }

            if (down && !_prevMouseDown)
            {
                if (_hover >= 0)
                {
                    auto action = _items[static_cast<std::size_t>(_hover)].action;
                    hide();
                    if (action) action();           // run AFTER hiding
                }
                else
                {
                    hide();                          // click outside dismisses
                }
            }
            _prevMouseDown = down;
        });
    }

    void ContextMenu::addItem(const std::string& label, std::function<void()> action)
    {
        _items.push_back({ label, std::move(action) });
        recomputeSize();
    }

    void ContextMenu::clearItems() { _items.clear(); recomputeSize(); }

    void ContextMenu::recomputeSize()
    {
        float maxW = 80.f;
        const sf::Font& font = FontManager<>::getDefault();
        for (const auto& it : _items)
        {
            sf::Text t(font, it.label, _charSize);
            maxW = std::max(maxW, t.getLocalBounds().size.x);
        }
        _width = maxW + _pad * 4.f;
        const float h = _items.empty() ? 0.f : (_items.size() * _itemH + _pad * 2.f);
        this->setSize({ _width, h });
    }

    void ContextMenu::showAt(const sf::Vector2f& pos)
    {
        _pos = pos;
        recomputeSize();
        _open = true;
        _prevMouseDown = true;       // ignore the click that opened it
        disableFlag(ml::Flag::HIDDEN);
    }

    void ContextMenu::hide()
    {
        const bool was = _open;
        _open  = false;
        _hover = -1;
        enableFlag(ml::Flag::HIDDEN);
        if (was && _onClose) _onClose();
    }

    void ContextMenu::onClose(std::function<void()> cb) { _onClose = std::move(cb); }

    sf::FloatRect ContextMenu::itemRect(std::size_t i) const
    {
        return { { _pos.x, _pos.y + _pad + static_cast<float>(i) * _itemH },
                 { _width, _itemH } };
    }

    void ContextMenu::setPosition(const sf::Vector2f& pos)
    {
        _pos = pos;
        Graphic<sf::RectangleShape>::setPosition(pos);
    }
    sf::Vector2f  ContextMenu::getPosition()     const { return _pos; }
    sf::FloatRect ContextMenu::getGlobalBounds() const { return { _pos, getSize() }; }

    void ContextMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (!_open || _items.empty()) return;

        sf::RectangleShape bg(getSize());
        bg.setPosition(_pos);
        bg.setFillColor(sf::Color(40, 40, 46));
        bg.setOutlineColor(sf::Color(80, 80, 90));
        bg.setOutlineThickness(1.f);
        target.draw(bg, states);

        const sf::Font& font = FontManager<>::getDefault();
        for (std::size_t i = 0; i < _items.size(); ++i)
        {
            const sf::FloatRect r = itemRect(i);
            if (static_cast<int>(i) == _hover)
            {
                sf::RectangleShape hl({ r.size.x, r.size.y });
                hl.setPosition(r.position);
                hl.setFillColor(sf::Color(70, 110, 200, 120));
                target.draw(hl, states);
            }
            sf::Text t(font, _items[i].label, _charSize);
            t.setFillColor(sf::Color(230, 230, 235));
            t.setPosition({ r.position.x + _pad * 2.f, r.position.y + 4.f });
            target.draw(t, states);
        }
    }

} // namespace ml
