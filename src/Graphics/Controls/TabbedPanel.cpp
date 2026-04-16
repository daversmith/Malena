// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/TabbedPanel.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <algorithm>
#include <cmath>

namespace ml
{
    TabbedPanel::TabbedPanel(const sf::Font& font_)
    {
        TabbedPanelTheme::applyFrom(ThemeManager::get());
        this->font = &font_;

        // ── Hover tracking + deferred content cleanup ─────────────────────────
        onUpdate([this]{
            // Safe to destroy here: Panel has no UPDATE subscription, so there
            // is no CLICK/HOVER subscriber entry being iterated right now.
            _pendingDelete.clear();

            const sf::Vector2f wp = WindowManager::getWindow().mapPixelToCoords(
                sf::Mouse::getPosition(WindowManager::getWindow()));
            _hoveredIdx = hitTestStrip(wp);
        });

        // ── Click — select tab or close ───────────────────────────────────────
        onClick([this]{
            const sf::Vector2f wp = WindowManager::getWindow().mapPixelToCoords(
                sf::Mouse::getPosition(WindowManager::getWindow()));

            // Check close buttons first
            for (int i = 0; i < static_cast<int>(_tabs.size()); ++i)
            {
                if ((_tabs[i].closeable || closeable) && hitTestClose(i, wp))
                {
                    const std::string lbl = _tabs[i].label;
                    removeTab(static_cast<std::size_t>(i));
                    if (_onTabClosed) _onTabClosed(static_cast<std::size_t>(i), lbl);
                    return;
                }
            }

            const int hit = hitTestStrip(wp);
            if (hit >= 0) selectTab(static_cast<std::size_t>(hit));
        });
    }

    // ── onThemeApplied ────────────────────────────────────────────────────────

    void TabbedPanel::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        TabbedPanelTheme::applyFrom(theme);
    }

    // ── Geometry ─────────────────────────────────────────────────────────────

    sf::FloatRect TabbedPanel::stripRect() const
    {
        switch (tabPosition)
        {
            case TabPosition::TOP:
                return {{_position.x, _position.y}, {_size.x, tabHeight}};
            case TabPosition::BOTTOM:
                return {{_position.x, _position.y + _size.y - tabHeight},
                        {_size.x, tabHeight}};
            case TabPosition::LEFT:
                return {{_position.x, _position.y}, {tabHeight, _size.y}};
            case TabPosition::RIGHT:
                return {{_position.x + _size.x - tabHeight, _position.y},
                        {tabHeight, _size.y}};
        }
        return {};
    }

    sf::FloatRect TabbedPanel::contentRect() const
    {
        switch (tabPosition)
        {
            case TabPosition::TOP:
                return {{_position.x, _position.y + tabHeight},
                        {_size.x, _size.y - tabHeight}};
            case TabPosition::BOTTOM:
                return {{_position.x, _position.y},
                        {_size.x, _size.y - tabHeight}};
            case TabPosition::LEFT:
                return {{_position.x + tabHeight, _position.y},
                        {_size.x - tabHeight, _size.y}};
            case TabPosition::RIGHT:
                return {{_position.x, _position.y},
                        {_size.x - tabHeight, _size.y}};
        }
        return {};
    }

    sf::Vector2f TabbedPanel::contentSize() const
    {
        const sf::FloatRect r = contentRect();
        return r.size;
    }

    sf::Vector2f TabbedPanel::stripAxis() const
    {
        return (tabPosition == TabPosition::LEFT ||
                tabPosition == TabPosition::RIGHT)
               ? sf::Vector2f{0.f, 1.f}
               : sf::Vector2f{1.f, 0.f};
    }

    // ── computeTabLayout ─────────────────────────────────────────────────────

    void TabbedPanel::computeTabLayout()
    {
        if (_tabs.empty()) return;

        const bool horiz = (tabPosition == TabPosition::TOP ||
                            tabPosition == TabPosition::BOTTOM);
        const float available = horiz ? _size.x : _size.y;
        const float charH = static_cast<float>(fontSize);

        // Measure each label
        sf::Text measure(*font, "", static_cast<unsigned int>(fontSize));
        float offset = padding;

        for (auto& tab : _tabs)
        {
            measure.setString(tab.label);
            float labelW = measure.getGlobalBounds().size.x;
            if (tab.icon) labelW += iconSize + 4.f;
            if (tab.closeable || closeable) labelW += closeSize + 4.f;

            tab.width = std::clamp(labelW + padding * 2.f, tabMinWidth, tabMaxWidth);
            tab.x     = offset;
            offset   += tab.width;
        }

        // Reposition and resize content for active tab
        if (_activeIdx >= 0 && _activeIdx < static_cast<int>(_tabs.size()))
        {
            const sf::FloatRect cr = contentRect();
            _tabs[_activeIdx].content->setPosition(cr.position);
            if (_tabs[_activeIdx].resizeFn)
                _tabs[_activeIdx].resizeFn(cr.size);
        }
    }

    // ── Hit testing ──────────────────────────────────────────────────────────

    int TabbedPanel::hitTestStrip(const sf::Vector2f& wp) const
    {
        const sf::FloatRect sr = stripRect();
        if (!sr.contains(wp)) return -1;

        const bool horiz = (tabPosition == TabPosition::TOP ||
                            tabPosition == TabPosition::BOTTOM);

        for (int i = 0; i < static_cast<int>(_tabs.size()); ++i)
        {
            const auto& tab = _tabs[i];
            sf::FloatRect tr;
            if (horiz)
                tr = {{sr.position.x + tab.x, sr.position.y}, {tab.width, tabHeight}};
            else
                tr = {{sr.position.x, sr.position.y + tab.x}, {tabHeight, tab.width}};

            if (tr.contains(wp)) return i;
        }
        return -1;
    }

    bool TabbedPanel::hitTestClose(int idx, const sf::Vector2f& wp) const
    {
        if (idx < 0 || idx >= static_cast<int>(_tabs.size())) return false;
        if (!_tabs[idx].closeable && !closeable) return false;

        const sf::FloatRect sr = stripRect();
        const bool horiz = (tabPosition == TabPosition::TOP ||
                            tabPosition == TabPosition::BOTTOM);
        const auto& tab  = _tabs[idx];

        float cx, cy;
        if (horiz)
        {
            cx = sr.position.x + tab.x + tab.width - closeSize - padding * 0.5f;
            cy = sr.position.y + (tabHeight - closeSize) / 2.f;
        }
        else
        {
            cx = sr.position.x + (tabHeight - closeSize) / 2.f;
            cy = sr.position.y + tab.x + tab.width - closeSize - padding * 0.5f;
        }

        return sf::FloatRect{{cx, cy}, {closeSize, closeSize}}.contains(wp);
    }

    // ── draw ──────────────────────────────────────────────────────────────────

    void TabbedPanel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // ── Content area background (skipped if transparent) ─────────────────
        const sf::FloatRect cr = contentRect();
        if (contentBg.a > 0)
        {
            sf::RectangleShape bg(cr.size);
            bg.setPosition(cr.position);
            bg.setFillColor(contentBg);
            bg.setOutlineColor(contentBorder);
            bg.setOutlineThickness(tabBarThickness);
            target.draw(bg, states);
        }

        // ── Tab strip ─────────────────────────────────────────────────────────
        drawStrip(target, states);

        // ── Active tab content (clipped to content rect) ──────────────────────
        if (_activeIdx >= 0 && _activeIdx < static_cast<int>(_tabs.size()))
        {
            auto* comp = _tabs[_activeIdx].content.get();
            if (comp)
            {
                const auto targetSize = target.getSize();
                const float tw = static_cast<float>(targetSize.x);
                const float th = static_cast<float>(targetSize.y);

                if (cr.size.x > 0.f && cr.size.y > 0.f)
                {
                    const sf::View savedView = target.getView();

                    sf::View contentView;
                    contentView.setCenter({cr.position.x + cr.size.x / 2.f,
                                           cr.position.y + cr.size.y / 2.f});
                    contentView.setSize(cr.size);
                    contentView.setViewport(sf::FloatRect{
                        {cr.position.x / tw, cr.position.y / th},
                        {cr.size.x / tw,     cr.size.y / th}
                    });

                    target.setView(contentView);
                    target.draw(*dynamic_cast<const sf::Drawable*>(comp), states);
                    target.setView(savedView);
                }
            }
        }
    }

    void TabbedPanel::drawStrip(sf::RenderTarget& target,
                                 const sf::RenderStates& states) const
    {
        const sf::FloatRect sr = stripRect();

        // Strip background
        sf::RectangleShape strip(sr.size);
        strip.setPosition(sr.position);
        strip.setFillColor(tabBarBg);
        target.draw(strip, states);

        for (int i = 0; i < static_cast<int>(_tabs.size()); ++i)
            drawTab(target, states, i,
                    i == _activeIdx,
                    i == _hoveredIdx && i != _activeIdx);

        // Divider line between strip and content
        const bool horiz = (tabPosition == TabPosition::TOP ||
                            tabPosition == TabPosition::BOTTOM);
        sf::RectangleShape div;
        if (tabPosition == TabPosition::TOP)
        {
            div.setSize({_size.x, tabBarThickness});
            div.setPosition({_position.x, _position.y + tabHeight - tabBarThickness});
        }
        else if (tabPosition == TabPosition::BOTTOM)
        {
            div.setSize({_size.x, tabBarThickness});
            div.setPosition({_position.x, _position.y + _size.y - tabHeight});
        }
        else if (tabPosition == TabPosition::LEFT)
        {
            div.setSize({tabBarThickness, _size.y});
            div.setPosition({_position.x + tabHeight - tabBarThickness, _position.y});
        }
        else
        {
            div.setSize({tabBarThickness, _size.y});
            div.setPosition({_position.x + _size.x - tabHeight, _position.y});
        }
        div.setFillColor(contentBorder);
        target.draw(div, states);
    }

    void TabbedPanel::drawTab(sf::RenderTarget& target,
                               const sf::RenderStates& states,
                               int idx, bool active, bool hovered) const
    {
        const sf::FloatRect sr = stripRect();
        const bool horiz = (tabPosition == TabPosition::TOP ||
                            tabPosition == TabPosition::BOTTOM);
        const auto& tab  = _tabs[idx];

        sf::FloatRect tr;
        if (horiz)
            tr = {{sr.position.x + tab.x, sr.position.y}, {tab.width, tabHeight}};
        else
            tr = {{sr.position.x, sr.position.y + tab.x}, {tabHeight, tab.width}};

        // Tab background
        sf::RectangleShape bg(tr.size);
        bg.setPosition(tr.position);
        bg.setFillColor(active ? tabActiveBg : (hovered ? tabHoverBg : tabIdleBg));
        target.draw(bg, states);

        // Active accent bar
        if (active)
        {
            sf::RectangleShape bar;
            const float barThick = 3.f;
            if (tabPosition == TabPosition::TOP)
            {
                bar.setSize({tr.size.x, barThick});
                bar.setPosition({tr.position.x, tr.position.y + tr.size.y - barThick});
            }
            else if (tabPosition == TabPosition::BOTTOM)
            {
                bar.setSize({tr.size.x, barThick});
                bar.setPosition(tr.position);
            }
            else if (tabPosition == TabPosition::LEFT)
            {
                bar.setSize({barThick, tr.size.y});
                bar.setPosition({tr.position.x + tr.size.x - barThick, tr.position.y});
            }
            else
            {
                bar.setSize({barThick, tr.size.y});
                bar.setPosition(tr.position);
            }
            bar.setFillColor(tabActiveBar);
            target.draw(bar, states);
        }

        // Label (and icon)
        float contentX = tr.position.x + padding;
        const float contentY = tr.position.y;
        const float rowH     = horiz ? tabHeight : tab.width;

        if (tab.icon)
        {
            sf::Sprite icon(*tab.icon);
            const auto native = tab.icon->getSize();
            const float scale = iconSize / std::max(
                static_cast<float>(native.x), static_cast<float>(native.y));
            icon.setScale({scale, scale});
            icon.setPosition({
                contentX,
                contentY + (rowH - iconSize) / 2.f
            });
            target.draw(icon, states);
            contentX += iconSize + 4.f;
        }

        sf::Text label(*font, tab.label, static_cast<unsigned int>(fontSize));
        label.setFillColor(active ? tabActiveText : tabIdleText);
        const sf::FloatRect lb = label.getLocalBounds();

        if (horiz)
        {
            label.setPosition({
                contentX,
                tr.position.y + (tabHeight - lb.size.y) / 2.f - lb.position.y
            });
        }
        else
        {
            // Rotate 90° for vertical tabs
            label.setRotation(sf::degrees(90.f));
            label.setPosition({
                tr.position.x + (tabHeight - lb.size.y) / 2.f - lb.position.y
                    + lb.size.y + lb.position.y,
                tr.position.y + tab.x + padding
            });
        }
        target.draw(label, states);

        // Close button
        if (tab.closeable || closeable)
        {
            float cx, cy;
            if (horiz)
            {
                cx = tr.position.x + tab.width - closeSize - padding * 0.5f;
                cy = tr.position.y + (tabHeight - closeSize) / 2.f;
            }
            else
            {
                cx = tr.position.x + (tabHeight - closeSize) / 2.f;
                cy = tr.position.y + tab.x + tab.width - closeSize - padding * 0.5f;
            }

            sf::Text x(*font, "x", static_cast<unsigned int>(fontSizeSmall));
            x.setFillColor(closeColor);
            const sf::FloatRect xb = x.getLocalBounds();
            x.setPosition({
                cx + (closeSize - xb.size.x) / 2.f - xb.position.x,
                cy + (closeSize - xb.size.y) / 2.f - xb.position.y
            });
            target.draw(x, states);
        }
    }

    // ── Tab management ────────────────────────────────────────────────────────

    void TabbedPanel::addTab(Tab tab)
    {
        _tabs.push_back(std::move(tab));
        computeTabLayout();

        if (_activeIdx < 0)
            selectTab(0);
    }

    void TabbedPanel::removeTab(std::size_t index)
    {
        if (index >= _tabs.size()) return;

        // Move content into the pending-delete list instead of destroying it
        // immediately. The CLICK dispatcher may still be iterating subscribers
        // that include this Panel's entry; keeping it alive avoids a
        // use-after-free when filter() dereferences the Core*.
        if (_tabs[index].content)
            _pendingDelete.push_back(std::move(_tabs[index].content));

        _tabs.erase(_tabs.begin() + static_cast<std::ptrdiff_t>(index));

        // Reset hover index — the mouse position recalculates it next frame.
        _hoveredIdx = -1;

        if (_tabs.empty())
        {
            _activeIdx = -1;
        }
        else
        {
            const int newIdx = std::min(
                _activeIdx, static_cast<int>(_tabs.size()) - 1);
            _activeIdx = -1; // force reselect
            selectTab(static_cast<std::size_t>(newIdx));
        }

        computeTabLayout();
    }

    void TabbedPanel::selectTab(std::size_t index)
    {
        if (index >= _tabs.size()) return;
        _activeIdx = static_cast<int>(index);

        const sf::FloatRect cr = contentRect();
        _tabs[index].content->setPosition(cr.position);
        if (_tabs[index].resizeFn)
            _tabs[index].resizeFn(cr.size);

        if (_onTabChanged)
            _onTabChanged(index, _tabs[index].label);
    }

    // ── Callbacks ─────────────────────────────────────────────────────────────

    void TabbedPanel::onTabChanged(
        std::function<void(std::size_t, const std::string&)> cb)
    { _onTabChanged = std::move(cb); }

    void TabbedPanel::onTabClosed(
        std::function<void(std::size_t, const std::string&)> cb)
    { _onTabClosed = std::move(cb); }

    // ── Size / position ───────────────────────────────────────────────────────

    void TabbedPanel::setSize(const sf::Vector2f& sz)
    {
        _size = sz;
        computeTabLayout();
    }

    void TabbedPanel::setPosition(const sf::Vector2f& pos)
    {
        _position = pos;
        computeTabLayout();
    }

    sf::Vector2f  TabbedPanel::getPosition()     const { return _position; }
    sf::FloatRect TabbedPanel::getGlobalBounds() const
    { return sf::FloatRect{_position, _size}; }

} // namespace ml
