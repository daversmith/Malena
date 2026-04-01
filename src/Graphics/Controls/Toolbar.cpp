// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/Toolbar.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>

namespace ml
{
    // ── A minimal internal button drawable ────────────────────────────────────
    // Toolbar buttons aren't full Components — they're plain drawables whose
    // hit-testing and hover state are managed by the Toolbar itself.

    Toolbar::Toolbar(const sf::Font& font_)
    {
        ToolbarTheme::applyFrom(ThemeManager::get());
        this->font = &font_;

        _barLength = static_cast<float>(WindowManager::getWindow().getSize().x);

        // ── Hover tracking ────────────────────────────────────────────────────
        onUpdate([this]{
            const sf::Vector2f wp = WindowManager::getWindow().mapPixelToCoords(
                sf::Mouse::getPosition(WindowManager::getWindow()));

            _hoveredIdx = -1;
            for (int i = 0; i < static_cast<int>(_items.size()); ++i)
            {
                if (_items[i].separator || !_items[i].owned) continue;
                if (_items[i].component->getGlobalBounds().contains(wp))
                {
                    _hoveredIdx = i;
                    break;
                }
            }
        });

        // ── Click — fire owned-button actions ─────────────────────────────────
        onClick([this]{
            const sf::Vector2f wp = WindowManager::getWindow().mapPixelToCoords(
                sf::Mouse::getPosition(WindowManager::getWindow()));

            for (auto& item : _items)
            {
                if (item.separator || !item.owned) continue;
                if (item.component->getGlobalBounds().contains(wp) && item.action)
                {
                    item.action();
                    return;
                }
            }
        });
    }

    // ── layout ────────────────────────────────────────────────────────────────

    void Toolbar::layout()
    {
        const bool horiz   = (orientation == Orientation::HORIZONTAL);
        const float thick  = getBarThickness();
        const float sepW   = 1.f;
        const float sepPad = 4.f;

        float offset = barPadding;

        for (auto& item : _items)
        {
            if (item.separator)
            {
                offset += sepPad + sepW + sepPad;
                continue;
            }

            sf::Vector2f pos;
            if (horiz)
            {
                // Center vertically in bar
                const float h = item.component->getGlobalBounds().size.y;
                pos = {_position.x + offset, _position.y + (thick - h) / 2.f};
                offset += item.component->getGlobalBounds().size.x + itemSpacing;
            }
            else
            {
                const float w = item.component->getGlobalBounds().size.x;
                pos = {_position.x + (thick - w) / 2.f, _position.y + offset};
                offset += item.component->getGlobalBounds().size.y + itemSpacing;
            }

            item.component->setPosition(pos);
        }
    }

    // ── onThemeApplied ────────────────────────────────────────────────────────

    void Toolbar::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        ToolbarTheme::applyFrom(theme);
    }

    // ── draw ──────────────────────────────────────────────────────────────────

    void Toolbar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        const bool  horiz = (orientation == Orientation::HORIZONTAL);
        const float thick = getBarThickness();

        // ── Bar background ────────────────────────────────────────────────────
        sf::RectangleShape bg;
        if (horiz)
            bg.setSize({_barLength, thick});
        else
            bg.setSize({thick, _barLength});
        bg.setFillColor(barBg);
        bg.setPosition(_position);
        target.draw(bg, states);

        // ── Items ─────────────────────────────────────────────────────────────
        float offset = barPadding;
        const float sepW   = 1.f;
        const float sepPad = 4.f;

        for (int i = 0; i < static_cast<int>(_items.size()); ++i)
        {
            const auto& item = _items[i];

            if (item.separator)
            {
                offset += sepPad;
                // Draw separator line
                sf::RectangleShape sep;
                if (horiz)
                {
                    sep.setSize({sepW, thick - barPadding * 2.f});
                    sep.setPosition({_position.x + offset, _position.y + barPadding});
                }
                else
                {
                    sep.setSize({thick - barPadding * 2.f, sepW});
                    sep.setPosition({_position.x + barPadding, _position.y + offset});
                }
                sep.setFillColor(separatorColor);
                target.draw(sep, states);
                offset += sepW + sepPad;
                continue;
            }

            // Hover highlight for owned buttons
            if (item.owned && i == _hoveredIdx)
            {
                const sf::FloatRect b = item.component->getGlobalBounds();
                sf::RectangleShape hl(b.size + sf::Vector2f{4.f, 4.f});
                hl.setFillColor(itemHoverBg);
                hl.setPosition(b.position - sf::Vector2f{2.f, 2.f});
                target.draw(hl, states);
            }

            // Draw the item
            target.draw(*dynamic_cast<const sf::Drawable*>(item.component), states);

            if (horiz)
                offset += item.component->getGlobalBounds().size.x + itemSpacing;
            else
                offset += item.component->getGlobalBounds().size.y + itemSpacing;
        }
    }

    // ── Adding items ──────────────────────────────────────────────────────────

    // Internal button — plain drawable, not a Component.
    // Toolbar manages all hit-testing and events; no framework registration needed.
    struct ToolbarButton final : public sf::Drawable, public ml::Core
    {
        sf::Text     text;
        sf::Vector2f size;
        sf::Vector2f pos;

        ToolbarButton(const sf::Font& f, const std::string& label,
                      const sf::Vector2f& sz, unsigned int charSz)
            : text(f, label, charSz), size(sz)
        {
            text.setFillColor(sf::Color::White);
            // Silence from event system — toolbar handles interaction
            this->unsubscribeAll();
        }

        void draw(sf::RenderTarget& t, sf::RenderStates s) const override
        {
            const sf::FloatRect lb = text.getLocalBounds();
            sf::Text tmp = text;
            tmp.setPosition({
                pos.x + (size.x - lb.size.x) / 2.f - lb.position.x,
                pos.y + (size.y - lb.size.y) / 2.f - lb.position.y
            });
            t.draw(tmp, s);
        }

        void          setPosition(const sf::Vector2f& p) override { pos = p; }
        sf::Vector2f  getPosition()     const override { return pos; }
        sf::FloatRect getGlobalBounds() const override { return {pos, size}; }
        sf::RenderStates getRenderStates() const override { return {}; }
    };

    void Toolbar::addButton(const std::string& label,
                             std::function<void()> action)
    {
        Item item;
        item.label  = label;
        item.action = std::move(action);

        // Measure label to compute natural width
        sf::Text measure(*font, label, static_cast<unsigned int>(fontSize));
        const float w = std::max(itemSize.x,
            measure.getGlobalBounds().size.x + padding * 2.f);
        const sf::Vector2f sz{w, itemSize.y};

        auto btn = std::make_unique<ToolbarButton>(
            *font, label, sz, static_cast<unsigned int>(fontSize));
        ml::Core* corePtr = btn.get();
        item.component = corePtr;
        item.owned     = std::unique_ptr<ml::Core>(btn.release());

        _items.push_back(std::move(item));
        layout();
    }

    void Toolbar::add(ml::Core& component)
    {
        Item item;
        item.component = &component;
        _items.push_back(std::move(item));
        layout();
    }

    void Toolbar::addSeparator()
    {
        Item item;
        item.separator = true;
        _items.push_back(std::move(item));
        // No layout needed — separator has no component
    }

    void Toolbar::clear()
    {
        _items.clear();
    }

    // ── Sizing ────────────────────────────────────────────────────────────────

    void Toolbar::setBarLength(float len)
    {
        _barLength = len;
        layout();
    }

    float Toolbar::getBarThickness() const
    {
        return (orientation == Orientation::HORIZONTAL)
               ? itemSize.y + barPadding * 2.f
               : itemSize.x + barPadding * 2.f;
    }

    // ── Positionable ──────────────────────────────────────────────────────────

    void Toolbar::setPosition(const sf::Vector2f& pos)
    {
        _position = pos;
        layout();
    }

    sf::Vector2f  Toolbar::getPosition()     const { return _position; }

    sf::FloatRect Toolbar::getGlobalBounds() const
    {
        const float thick = getBarThickness();
        if (orientation == Orientation::HORIZONTAL)
            return sf::FloatRect{_position, {_barLength, thick}};
        else
            return sf::FloatRect{_position, {thick, _barLength}};
    }

} // namespace ml
