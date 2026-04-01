// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/List.h>
#include <algorithm>

namespace ml
{
    List::List(const sf::Font& font_)
    {
        ListTheme::applyFrom(ThemeManager::get());
        this->font = &font_;

        _background.setFillColor(bgColor);
    }

    // ── onThemeApplied ────────────────────────────────────────────────────────

    void List::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        ListTheme::applyFrom(theme);
        _background.setFillColor(bgColor);
        applyThemeToOwnedItems();
        rebuildDividers();
    }

    // ── Adding rows ───────────────────────────────────────────────────────────

    ListItem& List::addItem(const std::string& label,
                             const std::string& description)
    {
        auto item = std::make_unique<ListItem>(*font);

        // Apply current list theme/settings to the new item
        {
            ListItemTheme t;
            t.applyFrom(ThemeManager::get());
            // Propagate list's font
            t.font = font;
            item->applyTheme(t);
        }

        item->setWidth(_width);
        item->setLabel(label);
        if (!description.empty())
            item->setDescription(description);

        ListItem* ptr = item.get();
        _rows.push_back({ptr, std::move(item)});

        layout();
        rebuildDividers();
        return *ptr;
    }

    void List::add(ml::Core& component)
    {
        // If it's a nested List, propagate indent
        if (auto* nested = dynamic_cast<List*>(&component))
            nested->setIndentOffset(_indent + indent);

        _rows.push_back({&component, nullptr});
        layout();
        rebuildDividers();
    }

    void List::clear()
    {
        _rows.clear();
        _dividers.clear();
    }

    // ── Internal helpers ──────────────────────────────────────────────────────

    void List::applyThemeToOwnedItems()
    {
        ListItemTheme t;
        t.applyFrom(ThemeManager::get());
        t.font = font;

        for (auto& row : _rows)
        {
            if (row.owned)
                row.owned->applyTheme(t);
        }
    }

    void List::applySettingsToOwnedItems()
    {
        for (auto& row : _rows)
        {
            if (row.owned)
            {
                row.owned->setWidth(_width);
                // Propagate slotGap from ListSettings if needed in future
            }
        }
    }

    // ── Layout ────────────────────────────────────────────────────────────────

    void List::layout()
    {
        if (_rows.empty()) return;

        float y = _position.y;
        const float x = _position.x + _indent;

        for (auto& row : _rows)
        {
            row.component->setPosition({x, y});
            y += row.component->getGlobalBounds().size.y;
        }

        // Resize background to cover all rows
        if (showBackground)
        {
            _background.setPosition({x, _position.y});
            _background.setSize({_width, y - _position.y});
            _background.setFillColor(bgColor);
        }
    }

    void List::rebuildDividers()
    {
        _dividers.clear();
        if (!showDividers || _rows.size() < 2) return;

        const float x = _position.x + _indent;

        for (std::size_t i = 0; i + 1 < _rows.size(); ++i)
        {
            const sf::FloatRect b = _rows[i].component->getGlobalBounds();
            const float divY      = b.position.y + b.size.y;

            sf::RectangleShape div({_width, dividerThickness});
            div.setPosition({x, divY - dividerThickness / 2.f});
            div.setFillColor(dividerColor);
            _dividers.push_back(div);
        }
    }

    // ── draw ──────────────────────────────────────────────────────────────────

    void List::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (showBackground)
            target.draw(_background, states);

        for (std::size_t i = 0; i < _rows.size(); ++i)
        {
            target.draw(*dynamic_cast<sf::Drawable*>(_rows[i].component), states);

            if (showDividers && i < _dividers.size())
                target.draw(_dividers[i], states);
        }
    }

    // ── Sizing ────────────────────────────────────────────────────────────────

    void List::setWidth(float w)
    {
        _width = w;
        for (auto& row : _rows)
            if (row.owned) row.owned->setWidth(w);
        layout();
        rebuildDividers();
    }

    float List::getTotalHeight() const
    {
        float h = 0.f;
        for (const auto& row : _rows)
            h += row.component->getGlobalBounds().size.y;
        return h;
    }

    // ── Positionable ──────────────────────────────────────────────────────────

    void List::setPosition(const sf::Vector2f& pos)
    {
        _position = pos;
        layout();
        rebuildDividers();
    }

    sf::Vector2f  List::getPosition()     const { return _position; }

    sf::FloatRect List::getGlobalBounds() const
    {
        return sf::FloatRect{
            {_position.x + _indent, _position.y},
            {_width, getTotalHeight()}
        };
    }

} // namespace ml
