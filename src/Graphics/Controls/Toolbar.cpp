// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0

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
                if (item.separator || !item.owned || !item.enabled) continue;
                if (item.component->getGlobalBounds().contains(wp) && item.action)
                {
                    item.action();
                    return;
                }
            }
        });

        onScroll([this](const std::optional<sf::Event>& event) {
            if (overflow != Overflow::SCROLL) return;
            if (!event) return;
            const auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>();
            if (!scroll) return;
            const float maxScroll = std::max(0.f, _totalItemsLen - _barLength + barPadding);
            _scrollOffsetX = std::clamp(_scrollOffsetX - scroll->delta * 30.f, 0.f, maxScroll);
            layout();
        });
    }

    // ── layout ────────────────────────────────────────────────────────────────

    void Toolbar::layout()
    {
        const bool horiz   = (orientation == Orientation::HORIZONTAL);
        const float sepW   = 1.f;
        const float sepPad = 4.f;

        // ── WRAP: flow items onto extra rows (H) / columns (V) within the bar length ─
        if (overflow == Overflow::WRAP && _barLength > 0.f)
        {
            const float rowStep = (horiz ? itemSize.y : itemSize.x) + itemSpacing;
            float along = barPadding;    // position along the bar axis
            int   row   = 0;
            for (auto& item : _items)
            {
                const float w = item.separator ? (sepPad + sepW + sepPad)
                    : (horiz ? item.component->getGlobalBounds().size.x
                             : item.component->getGlobalBounds().size.y);

                // Wrap before placing if it would exceed the bar length (but never
                // wrap when it's the first item on a row).
                if (along > barPadding && along + w > _barLength - barPadding)
                {
                    along = barPadding;
                    ++row;
                }
                if (!item.separator)
                {
                    const float cross = barPadding + static_cast<float>(row) * rowStep;
                    item.component->setPosition(horiz
                        ? sf::Vector2f{ _position.x + along,  _position.y + cross }
                        : sf::Vector2f{ _position.x + cross, _position.y + along });
                }
                along += w + (item.separator ? 0.f : itemSpacing);
            }
            _rowCount      = row + 1;
            _totalItemsLen = _barLength;
            return;
        }

        _rowCount          = 1;
        const float thick  = getBarThickness();

        const float scrollOffset = (horiz && overflow == Overflow::SCROLL) ? _scrollOffsetX : 0.f;
        float offset = barPadding - scrollOffset;

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

        _totalItemsLen = offset + scrollOffset - barPadding;
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
        const float scrollOff = (horiz && overflow == Overflow::SCROLL) ? _scrollOffsetX : 0.f;
        float offset = barPadding - scrollOff;
        const float sepW   = 1.f;
        const float sepPad = 4.f;

        const bool clipItems = (horiz && overflow == Overflow::SCROLL);
        const float clipLeft  = _position.x;
        const float clipRight = _position.x + _barLength;

        for (int i = 0; i < static_cast<int>(_items.size()); ++i)
        {
            const auto& item = _items[i];

            if (item.separator)
            {
                offset += sepPad;
                if (!clipItems || ((_position.x + offset) >= clipLeft && (_position.x + offset) <= clipRight))
                {
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
                }
                offset += sepW + sepPad;
                continue;
            }

            const sf::FloatRect b = item.component->getGlobalBounds();
            if (clipItems && (b.position.x + b.size.x < clipLeft || b.position.x > clipRight))
            {
                if (horiz) offset += b.size.x + itemSpacing;
                else       offset += b.size.y + itemSpacing;
                continue;
            }

            // Selected (active) highlight for owned buttons
            if (item.owned && item.selected)
            {
                sf::RectangleShape hl(b.size + sf::Vector2f{4.f, 4.f});
                hl.setFillColor(itemActiveBg);
                hl.setPosition(b.position - sf::Vector2f{2.f, 2.f});
                target.draw(hl, states);
            }

            // Hover highlight for owned buttons
            if (item.owned && i == _hoveredIdx && item.enabled)
            {
                sf::RectangleShape hl(b.size + sf::Vector2f{4.f, 4.f});
                hl.setFillColor(itemHoverBg);
                hl.setPosition(b.position - sf::Vector2f{2.f, 2.f});
                target.draw(hl, states);
            }

            // Draw the item
            target.draw(*dynamic_cast<const sf::Drawable*>(item.component), states);

            // Dim disabled items
            if (item.owned && !item.enabled)
            {
                sf::RectangleShape dim(b.size);
                dim.setFillColor({0, 0, 0, 120});
                dim.setPosition(b.position);
                target.draw(dim, states);
            }

            if (horiz)
                offset += b.size.x + itemSpacing;
            else
                offset += b.size.y + itemSpacing;
        }
    }

    // ── Adding items ──────────────────────────────────────────────────────────

    // Internal button — plain drawable, not a Component.
    // Toolbar manages all hit-testing and events; no framework registration needed.
    struct ToolbarButton final : public sf::Drawable, public ml::Core
    {
        sf::Text            text;
        sf::Vector2f        size;
        sf::Vector2f        pos;
        const sf::Texture*  icon    = nullptr;   // optional; not owned — must outlive us
        float               iconSz  = 0.f;
        float               iconGap = 8.f;

        ToolbarButton(const sf::Font& f, const std::string& label,
                      const sf::Vector2f& sz, unsigned int charSz)
            : text(f, sf::String::fromUtf8(label.begin(), label.end()), charSz), size(sz)
        {
            text.setFillColor(sf::Color::White);
            // Silence from event system — toolbar handles interaction
            this->unsubscribeAll();
        }

        void draw(sf::RenderTarget& t, sf::RenderStates s) const override
        {
            const sf::FloatRect lb = text.getLocalBounds();
            const bool  hasIcon = icon && iconSz > 0.f && icon->getSize().y > 0;
            const float iconW   = hasIcon ? iconSz : 0.f;
            const float gap     = hasIcon ? iconGap : 0.f;
            // Centre the [icon][gap][text] group in the button.
            const float groupW  = iconW + gap + lb.size.x;
            const float startX   = pos.x + (size.x - groupW) / 2.f;

            if (hasIcon)
            {
                sf::Sprite sp(*icon);
                const float sc = iconSz / static_cast<float>(icon->getSize().y);
                sp.setScale({ sc, sc });
                sp.setPosition({ startX, pos.y + (size.y - iconSz) / 2.f });
                t.draw(sp, s);
            }

            sf::Text tmp = text;
            tmp.setPosition({
                startX + iconW + gap - lb.position.x,
                pos.y + (size.y - lb.size.y) / 2.f - lb.position.y
            });
            t.draw(tmp, s);
        }

        void          setPosition(const sf::Vector2f& p) override { pos = p; }
        sf::Vector2f  getPosition()     const override { return pos; }
        sf::FloatRect getGlobalBounds() const override { return {pos, size}; }
        sf::RenderStates getRenderStates() const override { return {}; }
    };

    std::size_t Toolbar::addButton(const std::string& label,
                                    std::function<void()> action)
    {
        Item item;
        item.label  = label;
        item.action = std::move(action);

        // Measure label to compute natural width
        sf::Text measure(*font, sf::String::fromUtf8(label.begin(), label.end()),
                         static_cast<unsigned int>(fontSize));
        const float w = std::max(itemSize.x,
            measure.getGlobalBounds().size.x + padding * 2.f);
        const sf::Vector2f sz{w, itemSize.y};

        auto btn = std::make_unique<ToolbarButton>(
            *font, label, sz, static_cast<unsigned int>(fontSize));
        btn->text.setFillColor(itemTextColor);   // themeable (default white)
        ml::Core* corePtr = btn.get();
        item.component = corePtr;
        item.owned     = std::unique_ptr<ml::Core>(btn.release());

        const std::size_t idx = _items.size();
        _items.push_back(std::move(item));
        addComponent(*corePtr);
        layout();
        return idx;
    }

    std::size_t Toolbar::addButton(const std::string& label, const sf::Texture& icon,
                                    std::function<void()> action)
    {
        Item item;
        item.label  = label;
        item.action = std::move(action);

        const float iconSz  = itemSize.y * 0.58f;   // icon box within the bar height
        const float iconGap = 8.f;

        sf::Text measure(*font, sf::String::fromUtf8(label.begin(), label.end()),
                         static_cast<unsigned int>(fontSize));
        const float textW = measure.getGlobalBounds().size.x;
        const float w = std::max(itemSize.x, iconSz + iconGap + textW + padding * 2.f);
        const sf::Vector2f sz{w, itemSize.y};

        auto btn = std::make_unique<ToolbarButton>(
            *font, label, sz, static_cast<unsigned int>(fontSize));
        btn->text.setFillColor(itemTextColor);
        btn->icon    = &icon;
        btn->iconSz  = iconSz;
        btn->iconGap = iconGap;
        ml::Core* corePtr = btn.get();
        item.component = corePtr;
        item.owned     = std::unique_ptr<ml::Core>(btn.release());

        const std::size_t idx = _items.size();
        _items.push_back(std::move(item));
        addComponent(*corePtr);
        layout();
        return idx;
    }

    void Toolbar::setItemLabel(std::size_t index, const std::string& label)
    {
        if (index >= _items.size() || !_items[index].owned) return;
        _items[index].label = label;
        auto* btn = static_cast<ToolbarButton*>(_items[index].owned.get());
        btn->text.setString(sf::String::fromUtf8(label.begin(), label.end()));

        // Re-measure and resize
        sf::Text measure(*font, sf::String::fromUtf8(label.begin(), label.end()),
                         static_cast<unsigned int>(fontSize));
        const float w = std::max(itemSize.x,
            measure.getGlobalBounds().size.x + padding * 2.f);
        btn->size = {w, itemSize.y};
        layout();
    }

    void Toolbar::setItemEnabled(std::size_t index, bool enabled)
    {
        if (index >= _items.size()) return;
        _items[index].enabled = enabled;
    }

    void Toolbar::setItemSelected(std::size_t index, bool selected)
    {
        if (index >= _items.size()) return;
        _items[index].selected = selected;
    }

    void Toolbar::add(ml::Core& component)
    {
        Item item;
        item.component = &component;
        _items.push_back(std::move(item));
        addComponent(component);
        layout();
    }

    void Toolbar::add(const ml::Core& component)
    {
        add(const_cast<ml::Core&>(component));
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
        for (auto& item : _items)
            if (item.component) Core::removeComponent(*item.component);
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
        const float base = (orientation == Orientation::HORIZONTAL) ? itemSize.y : itemSize.x;
        const int   rows = (overflow == Overflow::WRAP) ? std::max(1, _rowCount) : 1;
        return base * rows + itemSpacing * (rows - 1) + barPadding * 2.f;
    }

    float Toolbar::getContentExtent() const
    {
        // _totalItemsLen is the summed item run (widths + spacings); add the
        // leading bar padding to measure from the bar origin to past the last item.
        return barPadding + _totalItemsLen;
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
