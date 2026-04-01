// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/MenuBar.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <algorithm>

namespace ml
{
    MenuBar::MenuBar(const sf::Font& font_)
    {
        MenuBarTheme::applyFrom(ThemeManager::get());
        this->font = &font_;

        // ── Click — open/close menus and activate items ───────────────────────
        onClick([this]{
            const sf::Vector2f wp = WindowManager::getWindow().mapPixelToCoords(
                sf::Mouse::getPosition(WindowManager::getWindow()));

            // Hit-test the bar
            const int barHit = hitTestBar(wp);
            if (barHit >= 0)
            {
                if (_openEntry == barHit) closeAll();
                else
                {
                    _openEntry   = barHit;
                    _hoveredItem = -1;
                    _openSubmenu = -1;
                    enableFlag(Flag::OPEN);
                    setState(State::OPEN);
                }
                return;
            }

            // Hit-test open dropdown
            if (_openEntry >= 0)
            {
                const int itemHit = hitTestDropdown(wp);
                if (itemHit >= 0)
                    activateItem(_openEntry, itemHit);
                else
                    closeAll(); // click outside
            }
        });

        // ── Update — hover tracking + keyboard navigation ─────────────────────
        onUpdate([this]{
            if (_openEntry < 0) return;

            const sf::Vector2f wp = WindowManager::getWindow().mapPixelToCoords(
                sf::Mouse::getPosition(WindowManager::getWindow()));

            // Hover in the bar: switch open menu on hover
            const int barHit = hitTestBar(wp);
            if (barHit >= 0 && barHit != _openEntry)
            {
                _openEntry   = barHit;
                _hoveredItem = -1;
                _openSubmenu = -1;
            }

            // Hover in dropdown
            _hoveredItem = hitTestDropdown(wp);
            if (_hoveredItem >= 0)
            {
                const auto& items = _entries[_openEntry].items;
                if (_hoveredItem < static_cast<int>(items.size()) &&
                    items[_hoveredItem].hasSubmenu())
                    _openSubmenu = _hoveredItem;
                else
                    _openSubmenu = -1;
            }
        });

        // ── Keyboard navigation ───────────────────────────────────────────────
        onKeypress([this](const std::optional<sf::Event>& e){
            if (_openEntry < 0 || !e) return;
            const auto* kp = e->getIf<sf::Event::KeyPressed>();
            if (!kp) return;

            const auto& items = _entries[_openEntry].items;

            if (kp->code == sf::Keyboard::Key::Escape)     { closeAll(); return; }
            if (kp->code == sf::Keyboard::Key::Left)
            {
                _openEntry = (_openEntry - 1 + static_cast<int>(_entries.size()))
                             % static_cast<int>(_entries.size());
                _hoveredItem = -1; return;
            }
            if (kp->code == sf::Keyboard::Key::Right)
            {
                _openEntry = (_openEntry + 1) % static_cast<int>(_entries.size());
                _hoveredItem = -1; return;
            }

            // Skip separators and disabled when navigating
            auto nextEnabled = [&](int dir) {
                int next = _hoveredItem + dir;
                while (next >= 0 && next < static_cast<int>(items.size()))
                {
                    if (!items[next].separator && items[next].enabled) return next;
                    next += dir;
                }
                return _hoveredItem;
            };

            if (kp->code == sf::Keyboard::Key::Down)
            {
                _hoveredItem = (_hoveredItem < 0) ? 0 : nextEnabled(+1);
                return;
            }
            if (kp->code == sf::Keyboard::Key::Up)
            {
                _hoveredItem = (_hoveredItem < 0)
                    ? static_cast<int>(items.size()) - 1
                    : nextEnabled(-1);
                return;
            }
            if (kp->code == sf::Keyboard::Key::Enter ||
                kp->code == sf::Keyboard::Key::Space)
            {
                if (_hoveredItem >= 0)
                    activateItem(_openEntry, _hoveredItem);
                return;
            }
        });

        computeLayout();
    }

    // ── Internal ──────────────────────────────────────────────────────────────

    void MenuBar::computeLayout()
    {
        if (autoFillWidth)
            _width = static_cast<float>(WindowManager::getWindow().getSize().x);

        float x = entryPadding;
        sf::Text measure(*font, "", static_cast<unsigned int>(fontSize));

        for (auto& entry : _entries)
        {
            measure.setString(entry.label);
            entry.labelX = x;
            entry.labelW = measure.getGlobalBounds().size.x;
            x += entry.labelW + entryPadding * 2.f;
        }
    }

    void MenuBar::closeAll()
    {
        _openEntry   = -1;
        _hoveredItem = -1;
        _openSubmenu = -1;
        disableFlag(Flag::OPEN);
        setState(State::IDLE);
    }

    int MenuBar::hitTestBar(const sf::Vector2f& wp) const
    {
        if (wp.y < _position.y || wp.y > _position.y + barHeight) return -1;
        for (int i = 0; i < static_cast<int>(_entries.size()); ++i)
        {
            const auto& e = _entries[i];
            const float left  = _position.x + e.labelX - entryPadding;
            const float right = left + e.labelW + entryPadding * 2.f;
            if (wp.x >= left && wp.x <= right) return i;
        }
        return -1;
    }

    int MenuBar::hitTestDropdown(const sf::Vector2f& wp) const
    {
        if (_openEntry < 0) return -1;
        const float dx = dropdownX(_openEntry);
        const float dy = dropdownY();
        const float dw = dropdownWidth(_openEntry);
        const float dh = dropdownHeight(_openEntry);

        if (wp.x < dx || wp.x > dx + dw ||
            wp.y < dy || wp.y > dy + dh) return -1;

        const float relY  = wp.y - dy;
        int   row   = 0;
        float accH  = 0.f;

        for (int i = 0; i < static_cast<int>(_entries[_openEntry].items.size()); ++i)
        {
            const float itemH = _entries[_openEntry].items[i].separator ? 9.f : dropdownItemH;
            if (relY >= accH && relY < accH + itemH) return i;
            accH += itemH;
        }
        return -1;
    }

    float MenuBar::dropdownWidth(int idx) const
    {
        if (idx < 0 || idx >= static_cast<int>(_entries.size())) return 0.f;
        sf::Text measure(*font, "", static_cast<unsigned int>(fontSize));
        float maxW = dropdownMinW;
        for (const auto& item : _entries[idx].items)
        {
            measure.setString(item.label);
            float w = measure.getGlobalBounds().size.x + padding * 2.f;
            if (!item.shortcut.empty())
            {
                measure.setString(item.shortcut);
                w += measure.getGlobalBounds().size.x + padding * 2.f;
            }
            if (item.hasSubmenu()) w += 14.f; // arrow
            maxW = std::max(maxW, w);
        }
        return maxW;
    }

    float MenuBar::dropdownHeight(int idx) const
    {
        if (idx < 0 || idx >= static_cast<int>(_entries.size())) return 0.f;
        float h = 0.f;
        for (const auto& item : _entries[idx].items)
            h += item.separator ? 9.f : dropdownItemH;
        return h;
    }

    float MenuBar::dropdownX(int idx) const
    {
        if (idx < 0 || idx >= static_cast<int>(_entries.size())) return 0.f;
        const float x = _position.x + _entries[idx].labelX - entryPadding;
        const float w = dropdownWidth(idx);
        const float winW = static_cast<float>(WindowManager::getWindow().getSize().x);
        return std::min(x, winW - w - 2.f);
    }

    float MenuBar::dropdownY() const
    {
        return _position.y + barHeight;
    }

    void MenuBar::activateItem(int entryIdx, int itemIdx)
    {
        if (entryIdx < 0 || entryIdx >= static_cast<int>(_entries.size())) return;
        auto& item = _entries[entryIdx].items[itemIdx];
        if (!item.enabled || item.separator) return;

        if (item.hasSubmenu()) return; // submenu handled by hover

        if (item.action)   item.action();
        closeAll();
    }

    // ── onThemeApplied ────────────────────────────────────────────────────────

    void MenuBar::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        MenuBarTheme::applyFrom(theme);
    }

    // ── draw ──────────────────────────────────────────────────────────────────

    void MenuBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // ── Bar background ────────────────────────────────────────────────────
        sf::RectangleShape bar({_width, barHeight});
        bar.setFillColor(barBg);
        bar.setPosition(_position);
        target.draw(bar, states);

        // ── Entry labels ──────────────────────────────────────────────────────
        sf::Text label(*font, "", static_cast<unsigned int>(fontSize));
        label.setFillColor(textColor);

        for (int i = 0; i < static_cast<int>(_entries.size()); ++i)
        {
            const auto& entry = _entries[i];
            const float left  = _position.x + entry.labelX - entryPadding;
            const float right = left + entry.labelW + entryPadding * 2.f;

            // Hover/open background
            if (i == _openEntry)
            {
                sf::RectangleShape hl({right - left, barHeight});
                hl.setFillColor(entryOpenBg);
                hl.setPosition({left, _position.y});
                target.draw(hl, states);
            }

            label.setString(entry.label);
            const sf::FloatRect lb = label.getLocalBounds();
            label.setPosition({
                _position.x + entry.labelX,
                _position.y + (barHeight - lb.size.y) / 2.f - lb.position.y
            });
            target.draw(label, states);
        }

        // ── Open dropdown ─────────────────────────────────────────────────────
        if (_openEntry >= 0)
            drawDropdown(target, states, _openEntry);
    }

    void MenuBar::drawDropdown(sf::RenderTarget& target,
                                const sf::RenderStates& states,
                                int entryIdx) const
    {
        const float dx = dropdownX(entryIdx);
        const float dy = dropdownY();
        const float dw = dropdownWidth(entryIdx);
        const float dh = dropdownHeight(entryIdx);

        // Panel
        sf::RectangleShape panel({dw, dh});
        panel.setFillColor(dropdownBg);
        panel.setOutlineColor(dropdownBorder);
        panel.setOutlineThickness(dropdownBorderThk);
        panel.setPosition({dx, dy});
        target.draw(panel, states);

        sf::Text label(*font, "", static_cast<unsigned int>(fontSize));
        sf::Text shortcut(*font, "", static_cast<unsigned int>(fontSizeSmall));
        shortcut.setFillColor(shortcutColor);

        float y = dy;
        for (int i = 0; i < static_cast<int>(_entries[entryIdx].items.size()); ++i)
        {
            const auto& item = _entries[entryIdx].items[i];

            if (item.separator)
            {
                sf::RectangleShape sep({dw - padding * 2.f, 1.f});
                sep.setFillColor(separatorColor);
                sep.setPosition({dx + padding, y + 4.f});
                target.draw(sep, states);
                y += 9.f;
                continue;
            }

            const float rowH = dropdownItemH;

            // Row highlight
            if (i == _hoveredItem && item.enabled)
            {
                sf::RectangleShape hl({dw, rowH});
                hl.setFillColor(itemHoverBg);
                hl.setPosition({dx, y});
                target.draw(hl, states);
            }

            // Checkmark
            if (item.checked)
            {
                sf::Text chk(*font, "✓", static_cast<unsigned int>(fontSize));
                chk.setFillColor(checkmarkColor);
                const sf::FloatRect cb = chk.getLocalBounds();
                chk.setPosition({dx + 6.f, y + (rowH - cb.size.y) / 2.f - cb.position.y});
                target.draw(chk, states);
            }

            // Label
            label.setString(item.label);
            label.setFillColor(item.enabled ? textColor : itemDisabledColor);
            const sf::FloatRect lb = label.getLocalBounds();
            label.setPosition({
                dx + padding + 18.f, // indent for checkmark
                y + (rowH - lb.size.y) / 2.f - lb.position.y
            });
            target.draw(label, states);

            // Shortcut
            if (!item.shortcut.empty())
            {
                shortcut.setString(item.shortcut);
                const sf::FloatRect sb = shortcut.getLocalBounds();
                shortcut.setPosition({
                    dx + dw - padding - sb.size.x - sb.position.x,
                    y + (rowH - sb.size.y) / 2.f - sb.position.y
                });
                target.draw(shortcut, states);
            }

            // Submenu arrow
            if (item.hasSubmenu())
            {
                sf::Text arrow(*font, "▶", static_cast<unsigned int>(fontSizeSmall));
                arrow.setFillColor(item.enabled ? textColor : itemDisabledColor);
                const sf::FloatRect ab = arrow.getLocalBounds();
                arrow.setPosition({
                    dx + dw - padding - ab.size.x - ab.position.x,
                    y + (rowH - ab.size.y) / 2.f - ab.position.y
                });
                target.draw(arrow, states);
            }

            y += rowH;
        }

        // Draw open submenu
        if (_openSubmenu >= 0)
            drawSubmenu(target, states, entryIdx, _openSubmenu);
    }

    void MenuBar::drawSubmenu(sf::RenderTarget& target,
                               const sf::RenderStates& states,
                               int entryIdx, int itemIdx) const
    {
        const auto& item = _entries[entryIdx].items[itemIdx];
        if (!item.hasSubmenu()) return;

        const float pdx   = dropdownX(entryIdx);
        const float pdw   = dropdownWidth(entryIdx);
        const float pdy   = dropdownY();

        // Compute item Y offset within the parent dropdown
        float itemY = pdy;
        for (int i = 0; i < itemIdx; ++i)
            itemY += _entries[entryIdx].items[i].separator ? 9.f : dropdownItemH;

        // Measure submenu
        float smW = dropdownMinW;
        {
            sf::Text m(*font, "", static_cast<unsigned int>(fontSize));
            for (const auto& si : item.submenu)
            {
                m.setString(si.label);
                smW = std::max(smW, m.getGlobalBounds().size.x + padding * 4.f);
            }
        }
        const float smH = item.submenu.size() * dropdownItemH;

        // Prefer opening to the right; flip left if near edge
        const float winW = static_cast<float>(WindowManager::getWindow().getSize().x);
        float smX = pdx + pdw - 1.f;
        if (smX + smW > winW) smX = pdx - smW + 1.f;
        const float smY = itemY;

        sf::RectangleShape panel({smW, smH});
        panel.setFillColor(dropdownBg);
        panel.setOutlineColor(dropdownBorder);
        panel.setOutlineThickness(dropdownBorderThk);
        panel.setPosition({smX, smY});
        target.draw(panel, states);

        sf::Text label(*font, "", static_cast<unsigned int>(fontSize));
        float y = smY;

        for (const auto& si : item.submenu)
        {
            if (!si.separator)
            {
                label.setString(si.label);
                label.setFillColor(si.enabled ? textColor : itemDisabledColor);
                const sf::FloatRect lb = label.getLocalBounds();
                label.setPosition({
                    smX + padding,
                    y + (dropdownItemH - lb.size.y) / 2.f - lb.position.y
                });
                target.draw(label, states);
            }
            y += si.separator ? 9.f : dropdownItemH;
        }
    }

    // ── Menu management ───────────────────────────────────────────────────────

    void MenuBar::addMenu(const std::string& label, std::vector<MenuItem> items)
    {
        _entries.push_back({label, std::move(items)});
        computeLayout();
    }

    void MenuBar::setMenuItems(std::size_t i, std::vector<MenuItem> items)
    {
        if (i < _entries.size())
            _entries[i].items = std::move(items);
    }

    void MenuBar::setItemChecked(std::size_t menu, std::size_t item, bool checked)
    {
        if (menu < _entries.size() && item < _entries[menu].items.size())
            _entries[menu].items[item].checked = checked;
    }

    void MenuBar::setItemEnabled(std::size_t menu, std::size_t item, bool enabled)
    {
        if (menu < _entries.size() && item < _entries[menu].items.size())
            _entries[menu].items[item].enabled = enabled;
    }

    void MenuBar::closeMenus() { closeAll(); }

    // ── Positionable ──────────────────────────────────────────────────────────

    void MenuBar::setPosition(const sf::Vector2f& pos)
    {
        _position = pos;
        computeLayout();
    }

    sf::Vector2f MenuBar::getPosition() const { return _position; }

    sf::FloatRect MenuBar::getGlobalBounds() const
    {
        // When a dropdown is open, extend bounds to cover it so clicks register
        if (_openEntry >= 0)
        {
            const float dh = dropdownHeight(_openEntry);
            return sf::FloatRect{_position, {_width, barHeight + dh + 4.f}};
        }
        return sf::FloatRect{_position, {_width, barHeight}};
    }

} // namespace ml
