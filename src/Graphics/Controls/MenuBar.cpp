// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/MenuBar.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>

// ── MenuSeparator ─────────────────────────────────────────────────────────────
// A 1-pixel horizontal rule that reserves 9 px of vertical space in the List
// layout (4 px padding above and below the line). Inherits ml::Rectangle for
// drawing but overrides setPosition / getGlobalBounds so the List engine sees
// the full 9 px allocation while the actual shape is centred within it.
// unsubscribeAll() is called in the constructor so it never intercepts events.

namespace
{
    class MenuSeparator : public ml::Rectangle
    {
        sf::Vector2f _logicalPos;
        float        _width;

    public:
        MenuSeparator(float width, const sf::Color& color)
            : _width(width)
        {
            setSize({width, 1.f});
            setFillColor(color);
            unsubscribeAll();
        }

        void setPosition(const sf::Vector2f& pos) override
        {
            _logicalPos = pos;
            ml::Rectangle::setPosition({pos.x, pos.y + 4.f}); // centre the 1-px line
        }

        sf::Vector2f  getPosition()     const override { return _logicalPos; }
        sf::FloatRect getGlobalBounds() const override
        { return {_logicalPos, {_width, 9.f}}; }
    };
} // anonymous namespace

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
                    if (_openEntry >= 0)
                        _entries[_openEntry].dropdown->setPosition({-9999.f, -9999.f});
                    _openEntry   = barHit;
                    _hoveredItem = -1;
                    _openSubmenu = -1;
                    _entries[_openEntry].dropdown->setPosition(
                        {dropdownX(_openEntry), dropdownY()});
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

        // ── Update — deferred cleanup + hover tracking + keyboard navigation ─────
        onUpdate([this]{
            // Release any Lists that were replaced during a CLICK dispatch.
            // Safe here because UPDATE fires outside the busy-depth window.
            _pendingDelete.clear();

            if (_openEntry < 0) return;

            const sf::Vector2f wp = WindowManager::getWindow().mapPixelToCoords(
                sf::Mouse::getPosition(WindowManager::getWindow()));

            // Hover in the bar: switch open menu on hover
            const int barHit = hitTestBar(wp);
            if (barHit >= 0 && barHit != _openEntry)
            {
                _entries[_openEntry].dropdown->setPosition({-9999.f, -9999.f});
                _openEntry   = barHit;
                _hoveredItem = -1;
                _openSubmenu = -1;
                _entries[_openEntry].dropdown->setPosition(
                    {dropdownX(_openEntry), dropdownY()});
            }

            // Hover in dropdown (for submenu tracking only; ListItem handles visual hover)
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
                _entries[_openEntry].dropdown->setPosition({-9999.f, -9999.f});
                _openEntry = (_openEntry - 1 + static_cast<int>(_entries.size()))
                             % static_cast<int>(_entries.size());
                _entries[_openEntry].dropdown->setPosition(
                    {dropdownX(_openEntry), dropdownY()});
                _hoveredItem = -1; return;
            }
            if (kp->code == sf::Keyboard::Key::Right)
            {
                _entries[_openEntry].dropdown->setPosition({-9999.f, -9999.f});
                _openEntry = (_openEntry + 1) % static_cast<int>(_entries.size());
                _entries[_openEntry].dropdown->setPosition(
                    {dropdownX(_openEntry), dropdownY()});
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

        float x = leftInset + entryPadding;
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
        if (_openEntry >= 0 && _openEntry < static_cast<int>(_entries.size()) &&
            _entries[_openEntry].dropdown)
            _entries[_openEntry].dropdown->setPosition({-9999.f, -9999.f});
        _openEntry   = -1;
        _hoveredItem = -1;
        _openSubmenu = -1;
        disableFlag(Flag::OPEN);
        setState(State::IDLE);
    }

    void MenuBar::buildDropdown(int entryIdx)
    {
        auto& entry = _entries[entryIdx];
        const float dw = dropdownWidth(entryIdx);

        // Move the entire old dropdown bundle into the pending-delete queue.
        // The List's ListItems hold raw _start/_end pointers into checkmarkTexts
        // and endTexts; ThemeManager will re-notify the still-living List before
        // we return, so all three vectors must outlive each other.
        // Everything is released safely on the next UPDATE frame.
        if (entry.dropdown)
        {
            OldDropdown old;
            old.dropdown      = std::move(entry.dropdown);
            old.checkmarkTexts = std::move(entry.checkmarkTexts);
            old.endTexts       = std::move(entry.endTexts);
            old.separatorRects = std::move(entry.separatorRects);
            _pendingDelete.push_back(std::move(old));
        }
        entry.listItems.clear();

        // Create the ml::List that renders the dropdown
        entry.dropdown = std::make_unique<ml::List>(*font);
        entry.dropdown->showBackground = true;
        entry.dropdown->showDividers   = false;
        entry.dropdown->bgColor        = dropdownBg;
        entry.dropdown->setWidth(dw);
        entry.dropdown->setPosition({-9999.f, -9999.f});

        // Build a ListItemTheme that matches the MenuBar's dropdown appearance
        ml::ListItemTheme itemTheme;
        itemTheme.bgIdle            = sf::Color::Transparent;
        itemTheme.bgHover           = itemHoverBg;
        itemTheme.bgDisabled        = sf::Color::Transparent;
        itemTheme.textColor         = textColor;
        itemTheme.disabledTextColor = itemDisabledColor;
        itemTheme.font              = font;
        itemTheme.fontSize          = fontSize;
        itemTheme.fontSizeSmall     = fontSizeSmall;
        itemTheme.padding           = padding;

        for (const auto& item : entry.items)
        {
            if (item.separator)
            {
                auto sep = std::unique_ptr<ml::Core>(new MenuSeparator(dw, separatorColor));
                entry.dropdown->add(*sep);
                entry.separatorRects.push_back(std::move(sep));
                // Null placeholders in the parallel vectors
                entry.listItems.push_back(nullptr);
                entry.checkmarkTexts.push_back(nullptr);
                entry.endTexts.push_back(nullptr);
            }
            else
            {
                auto& li = entry.dropdown->addItem(item.label);
                li.setEnabled(item.enabled);
                li.setRowHeight(dropdownItemH);
                li.applyTheme(itemTheme);

                // Checkmark start slot — always "✓" but transparent when unchecked,
                // so all labels are consistently indented.
                auto chk = std::make_unique<ml::Text>(*font);
                chk->setString("✓");
                chk->setCharacterSize(fontSize);
                chk->setFillColor(item.checked ? checkmarkColor : sf::Color::Transparent);
                chk->unsubscribeAll();
                li.setStart(*chk);
                entry.checkmarkTexts.push_back(std::move(chk));

                // End slot: submenu arrow or shortcut text
                if (item.hasSubmenu())
                {
                    auto arrow = std::make_unique<ml::Text>(*font);
                    arrow->setString("▶");
                    arrow->setCharacterSize(fontSizeSmall);
                    arrow->setFillColor(item.enabled ? textColor : itemDisabledColor);
                    arrow->unsubscribeAll();
                    li.setEnd(*arrow);
                    entry.endTexts.push_back(std::move(arrow));
                }
                else if (!item.shortcut.empty())
                {
                    auto sc = std::make_unique<ml::Text>(*font);
                    sc->setString(item.shortcut);
                    sc->setCharacterSize(fontSizeSmall);
                    sc->setFillColor(item.enabled ? shortcutColor : itemDisabledColor);
                    sc->unsubscribeAll();
                    li.setEnd(*sc);
                    entry.endTexts.push_back(std::move(sc));
                }
                else
                {
                    entry.endTexts.push_back(nullptr);
                }

                entry.listItems.push_back(&li);
            }
        }
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
        if (_openEntry < 0 || !_entries[_openEntry].dropdown) return -1;
        const sf::FloatRect lb = _entries[_openEntry].dropdown->getGlobalBounds();
        if (!lb.contains(wp)) return -1;

        const float relY = wp.y - lb.position.y;
        float accH = 0.f;
        for (int i = 0; i < static_cast<int>(_entries[_openEntry].items.size()); ++i)
        {
            const float itemH = _entries[_openEntry].items[i].separator
                                ? 9.f : dropdownItemH;
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
            if (item.hasSubmenu()) w += 14.f;
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
        closeAll(); // move any open dropdown off-screen before rebuilding
        MenuBarTheme::applyFrom(theme);
        for (int i = 0; i < static_cast<int>(_entries.size()); ++i)
            buildDropdown(i);
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

        // ── Open dropdown (rendered via ml::List) ─────────────────────────────
        if (_openEntry >= 0 && _openEntry < static_cast<int>(_entries.size()) &&
            _entries[_openEntry].dropdown)
        {
            const auto& dropdown = _entries[_openEntry].dropdown;
            const sf::FloatRect lb = dropdown->getGlobalBounds();

            // Background border: slightly larger rect in border color, then
            // draw the list on top so the border appears as a 1-px frame.
            const float t = dropdownBorderThk;
            sf::RectangleShape borderRect(
                {lb.size.x + t * 2.f, lb.size.y + t * 2.f});
            borderRect.setFillColor(dropdownBorder);
            borderRect.setPosition({lb.position.x - t, lb.position.y - t});
            target.draw(borderRect, states);

            if (const auto* d = dynamic_cast<const sf::Drawable*>(dropdown.get()))
                target.draw(*d, states);

            // Submenu (manual rendering — submenus are less common and need
            // different positioning logic than a flat dropdown)
            if (_openSubmenu >= 0)
                drawSubmenu(target, states, _openEntry, _openSubmenu);
        }
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
        const float smH = static_cast<float>(item.submenu.size()) * dropdownItemH;

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

        sf::Text lbl(*font, "", static_cast<unsigned int>(fontSize));
        float y = smY;

        for (const auto& si : item.submenu)
        {
            if (!si.separator)
            {
                lbl.setString(si.label);
                lbl.setFillColor(si.enabled ? textColor : itemDisabledColor);
                const sf::FloatRect b = lbl.getLocalBounds();
                lbl.setPosition({
                    smX + padding,
                    y + (dropdownItemH - b.size.y) / 2.f - b.position.y
                });
                target.draw(lbl, states);
            }
            y += si.separator ? 9.f : dropdownItemH;
        }
    }

    // ── Menu management ───────────────────────────────────────────────────────

    void MenuBar::addMenu(const std::string& label, std::vector<MenuItem> items)
    {
        _entries.push_back({label, std::move(items)});
        computeLayout();
        buildDropdown(static_cast<int>(_entries.size()) - 1);
    }

    void MenuBar::setMenuItems(std::size_t i, std::vector<MenuItem> items)
    {
        if (i < _entries.size())
        {
            _entries[i].items = std::move(items);
            buildDropdown(static_cast<int>(i));
        }
    }

    void MenuBar::setItemChecked(std::size_t menu, std::size_t item, bool checked)
    {
        if (menu >= _entries.size() || item >= _entries[menu].items.size()) return;
        auto& entry = _entries[menu];
        entry.items[item].checked = checked;
        if (item < entry.checkmarkTexts.size() && entry.checkmarkTexts[item])
            entry.checkmarkTexts[item]->setFillColor(
                checked ? checkmarkColor : sf::Color::Transparent);
    }

    void MenuBar::setItemEnabled(std::size_t menu, std::size_t item, bool enabled)
    {
        if (menu >= _entries.size() || item >= _entries[menu].items.size()) return;
        auto& entry = _entries[menu];
        entry.items[item].enabled = enabled;
        if (item < entry.listItems.size() && entry.listItems[item])
            entry.listItems[item]->setEnabled(enabled);
        if (item < entry.endTexts.size() && entry.endTexts[item])
        {
            const sf::Color col = enabled
                ? (entry.items[item].hasSubmenu() ? textColor : shortcutColor)
                : itemDisabledColor;
            entry.endTexts[item]->setFillColor(col);
        }
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
        if (_openEntry >= 0 && _openEntry < static_cast<int>(_entries.size()) &&
            _entries[_openEntry].dropdown)
        {
            const float dh = _entries[_openEntry].dropdown->getTotalHeight();
            return sf::FloatRect{_position, {_width, barHeight + dh + 4.f}};
        }
        return sf::FloatRect{_position, {_width, barHeight}};
    }

} // namespace ml
