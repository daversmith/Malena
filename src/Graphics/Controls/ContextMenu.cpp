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
#include <cctype>

namespace ml
{
    static std::string toLower(const std::string& s)
    {
        std::string r = s;
        for (char& c : r) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return r;
    }

    ContextMenu::ContextMenu()
    {
        this->setFillColor(sf::Color::Transparent);
        hide();

        onUpdate([this] {
            if (!_open || !AppManager::isUnderExclusiveOwner(this))
            { _prevMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left); return; }

            recomputeSize();   // reflect the current filter (typing narrows the list)

            const auto& win = WindowManager::getWindow();
            const sf::Vector2f wp = win.mapPixelToCoords(sf::Mouse::getPosition(win));
            const bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

            _hover = -1;
            const int vis = visibleCount();
            for (int vi = 0; vi < vis; ++vi)
                if (rowRect(vi).contains(wp)) { _hover = vi; break; }

            if (down && !_prevMouseDown)
            {
                const bool inSearch = _searchable && searchRect().contains(wp);
                if (_hover >= 0)
                {
                    const std::size_t item = _filtered[static_cast<std::size_t>(_scrollItem + _hover)];
                    auto action = _items[item].action;
                    hide();
                    if (action) action();           // run AFTER hiding
                }
                else if (!inSearch && !getGlobalBounds().contains(wp))
                {
                    hide();                          // click outside dismisses
                }
                // a click on the search row / padding keeps the menu open
            }
            _prevMouseDown = down;
        });

        // ── Type-to-filter. KeyableDispatcher broadcasts KEYPRESS/TEXT_ENTERED to
        //    every enabled, non-hidden subscriber (each self-guards on focus), so an
        //    open menu captures typing directly — no child text field needed. ──
        onTextEntered([this](const std::optional<sf::Event>& e) {
            if (!_open || !_searchable || !e) return;
            if (const auto* te = e->getIf<sf::Event::TextEntered>())
            {
                const char32_t u = te->unicode;
                if (u >= 32 && u < 127) { _filter += static_cast<char>(u); _scrollItem = 0; }
            }
        });
        onKeypress([this](const std::optional<sf::Event>& e) {
            if (!_open || !e) return;
            if (const auto* kp = e->getIf<sf::Event::KeyPressed>())
            {
                if (kp->code == sf::Keyboard::Key::Backspace)
                { if (_searchable && !_filter.empty()) { _filter.pop_back(); _scrollItem = 0; } }
                else if (kp->code == sf::Keyboard::Key::Escape) hide();
            }
        });
        onScroll([this](const std::optional<sf::Event>& e) {
            if (!_open || !e) return;
            if (const auto* sc = e->getIf<sf::Event::MouseWheelScrolled>())
            { _scrollItem -= (sc->delta > 0.f ? 1 : -1); clampScroll(); }
        });
    }

    void ContextMenu::addItem(const std::string& label, std::function<void()> action)
    {
        _items.push_back({ label, std::move(action) });
        recomputeSize();
    }

    void ContextMenu::clearItems()
    {
        _items.clear(); _filter.clear(); _scrollItem = 0;
        recomputeSize();
    }

    void ContextMenu::setSearchable(bool on)    { _searchable = on; recomputeSize(); }
    void ContextMenu::setMaxVisibleItems(int n) { _maxVisible = std::max(1, n); recomputeSize(); }

    void ContextMenu::recomputeFiltered()
    {
        _filtered.clear();
        const std::string needle = toLower(_filter);
        for (std::size_t i = 0; i < _items.size(); ++i)
            if (needle.empty() || toLower(_items[i].label).find(needle) != std::string::npos)
                _filtered.push_back(i);
    }

    void ContextMenu::clampScroll()
    {
        const int maxScroll = std::max(0, static_cast<int>(_filtered.size()) - _maxVisible);
        _scrollItem = std::max(0, std::min(_scrollItem, maxScroll));
    }

    int ContextMenu::visibleCount() const
    {
        return std::min(_maxVisible, static_cast<int>(_filtered.size()) - _scrollItem);
    }

    float ContextMenu::itemsTop() const
    {
        return _pos.y + _pad + (_searchable ? _searchH : 0.f);
    }

    void ContextMenu::recomputeSize()
    {
        float maxW = 80.f;
        const sf::Font& font = FontManager<>::getDefault();
        for (const auto& it : _items)   // width from ALL items so it doesn't jump while filtering
        {
            sf::Text t(font, it.label, _charSize);
            maxW = std::max(maxW, t.getLocalBounds().size.x);
        }
        _width = maxW + _pad * 4.f;

        recomputeFiltered();
        clampScroll();
        const int vis = _filtered.empty() ? 1 : visibleCount();   // 1 row for "(no matches)"
        const float h = _pad * 2.f + (_searchable ? _searchH : 0.f) + static_cast<float>(vis) * _itemH;
        this->setSize({ _width, h });
    }

    void ContextMenu::showAt(const sf::Vector2f& pos)
    {
        _pos = pos;
        _filter.clear();
        _scrollItem = 0;
        recomputeSize();
        _open = true;
        _prevMouseDown = true;       // ignore the click that opened it
        disableFlag(ml::Flag::HIDDEN);

        _prevPopup = AppManager::activePopup();
        _prevOwner = AppManager::exclusiveOwner();
        AppManager::setActivePopup(this);
        AppManager::setExclusiveOwner(this);
    }

    void ContextMenu::hide()
    {
        const bool was = _open;
        _open  = false;
        _hover = -1;
        enableFlag(ml::Flag::HIDDEN);

        if (was)
        {
            if (AppManager::activePopup()    == this) AppManager::setActivePopup(_prevPopup);
            if (AppManager::exclusiveOwner() == this) AppManager::setExclusiveOwner(_prevOwner);
        }
        if (was && _onClose) _onClose();
    }

    void ContextMenu::onClose(std::function<void()> cb) { _onClose = std::move(cb); }

    sf::FloatRect ContextMenu::searchRect() const
    {
        return { { _pos.x, _pos.y + _pad }, { _width, _searchH } };
    }

    sf::FloatRect ContextMenu::rowRect(int vi) const
    {
        return { { _pos.x, itemsTop() + static_cast<float>(vi) * _itemH }, { _width, _itemH } };
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
        if (!_open) return;

        sf::RectangleShape bg(getSize());
        bg.setPosition(_pos);
        bg.setFillColor(sf::Color(40, 40, 46));
        bg.setOutlineColor(sf::Color(80, 80, 90));
        bg.setOutlineThickness(1.f);
        target.draw(bg, states);

        const sf::Font& font = FontManager<>::getDefault();

        // ── search box ──
        if (_searchable)
        {
            const sf::FloatRect sr = searchRect();
            sf::RectangleShape box({ sr.size.x - _pad * 2.f, sr.size.y - _pad * 2.f });
            box.setPosition({ sr.position.x + _pad, sr.position.y + _pad });
            box.setFillColor(sf::Color(28, 28, 33));
            box.setOutlineColor(sf::Color(90, 90, 105));
            box.setOutlineThickness(1.f);
            target.draw(box, states);

            const bool empty = _filter.empty();
            sf::Text t(font, empty ? std::string("Search...") : _filter, _charSize);
            t.setFillColor(empty ? sf::Color(130, 130, 140) : sf::Color(235, 235, 240));
            t.setPosition({ sr.position.x + _pad * 3.f, sr.position.y + 6.f });
            target.draw(t, states);
        }

        // ── rows (filtered + scrolled) ──
        const int vis = visibleCount();
        if (_filtered.empty())
        {
            sf::Text t(font, std::string("(no matches)"), _charSize);
            t.setFillColor(sf::Color(140, 140, 150));
            t.setPosition({ _pos.x + _pad * 2.f, itemsTop() + 4.f });
            target.draw(t, states);
        }
        for (int vi = 0; vi < vis; ++vi)
        {
            const sf::FloatRect r = rowRect(vi);
            if (vi == _hover)
            {
                sf::RectangleShape hl({ r.size.x, r.size.y });
                hl.setPosition(r.position);
                hl.setFillColor(sf::Color(70, 110, 200, 120));
                target.draw(hl, states);
            }
            const std::size_t item = _filtered[static_cast<std::size_t>(_scrollItem + vi)];
            sf::Text t(font, _items[item].label, _charSize);
            t.setFillColor(sf::Color(230, 230, 235));
            t.setPosition({ r.position.x + _pad * 2.f, r.position.y + 4.f });
            target.draw(t, states);
        }

        // ── scrollbar (only when the list overflows) ──
        const int total = static_cast<int>(_filtered.size());
        if (total > _maxVisible)
        {
            const float viewTop = itemsTop();
            const float viewH   = static_cast<float>(vis) * _itemH;
            const float trackW  = 4.f;
            const float trackX  = _pos.x + _width - trackW - 2.f;
            sf::RectangleShape track({ trackW, viewH });
            track.setPosition({ trackX, viewTop });
            track.setFillColor(sf::Color(60, 60, 68));
            target.draw(track, states);
            const float denom  = static_cast<float>(total - _maxVisible);
            const float thumbH = std::max(18.f, viewH * static_cast<float>(_maxVisible) / static_cast<float>(total));
            const float thumbY = viewTop + (viewH - thumbH) * (denom > 0.f ? static_cast<float>(_scrollItem) / denom : 0.f);
            sf::RectangleShape thumb({ trackW, thumbH });
            thumb.setPosition({ trackX, thumbY });
            thumb.setFillColor(sf::Color(130, 130, 145));
            target.draw(thumb, states);
        }
    }

} // namespace ml
