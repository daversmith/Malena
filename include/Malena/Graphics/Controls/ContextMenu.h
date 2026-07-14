// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#ifndef MALENA_CONTEXTMENU_H
#define MALENA_CONTEXTMENU_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Graphics/Base/Graphic.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <functional>
#include <string>
#include <vector>

namespace ml
{
    /**
     * @brief A reusable right-click / pop-up menu: a small list of labeled
     *        items shown at a point that runs an action on click and dismisses
     *        on selection or click-outside.
     * @ingroup GraphicsControls
     *
     * Self-contained (mouse-poll), so it works anywhere. Add it LAST to its
     * parent (or draw last) so it overlays other content.
     *
     * @code
     * ml::ContextMenu menu;
     * menu.addItem("Delete", [&]{ list.remove(i); });
     * // on right-click:
     * menu.showAt(mousePos);
     * @endcode
     */
    class MALENA_API ContextMenu : public Graphic<sf::RectangleShape>
    {
    public:
        ContextMenu();

        void addItem(const std::string& label, std::function<void()> action);
        void clearItems();

        /** Opt in to a filter box at the top + wheel-scrolling for long lists.
         *  Type while the menu is open to narrow it; Backspace edits, Esc closes. */
        void setSearchable(bool on);
        /** Max rows shown before the list scrolls (default 12). */
        void setMaxVisibleItems(int n);

        void showAt(const sf::Vector2f& pos);
        void hide();
        [[nodiscard]] bool isOpen() const { return _open; }

        /** Fired when the menu closes (after a selection or click-outside). */
        void onClose(std::function<void()> cb);

        void setPosition(const sf::Vector2f& pos) override;
        [[nodiscard]] sf::Vector2f  getPosition()     const override;
        [[nodiscard]] sf::FloatRect getGlobalBounds() const override;

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    private:
        struct Item { std::string label; std::function<void()> action; };

        [[nodiscard]] sf::FloatRect rowRect(int visibleRow) const;  // vi-th shown row
        [[nodiscard]] sf::FloatRect searchRect() const;
        [[nodiscard]] float itemsTop() const;                       // y where rows begin
        [[nodiscard]] int   visibleCount() const;                   // rows shown this frame
        void recomputeFiltered();                                   // rebuild _filtered from _filter
        void clampScroll();
        void recomputeSize();

        std::vector<Item> _items;
        bool         _open          = false;
        bool         _prevMouseDown = false;
        sf::Vector2f _pos  { 0.f, 0.f };
        float        _itemH = 26.f;
        float        _pad   = 4.f;
        float        _width = 140.f;
        unsigned int _charSize = 14;
        mutable int  _hover = -1;
        // Search + scroll (search is opt-in via setSearchable).
        bool         _searchable = false;
        std::string  _filter;
        float        _searchH   = 30.f;
        int          _maxVisible = 12;
        int          _scrollItem = 0;                 // first visible filtered row
        std::vector<std::size_t> _filtered;           // item indices matching _filter
        std::function<void()> _onClose;
        // While open, the menu claims the popup + exclusive-owner layers so it draws
        // on top and blocks background clicks; these hold the prior holders to restore.
        Core* _prevOwner = nullptr;
        Core* _prevPopup = nullptr;
    };

} // namespace ml

#endif // MALENA_CONTEXTMENU_H
