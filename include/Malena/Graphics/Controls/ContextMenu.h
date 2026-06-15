// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

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

        [[nodiscard]] sf::FloatRect itemRect(std::size_t i) const;
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
        std::function<void()> _onClose;
    };

} // namespace ml

#endif // MALENA_CONTEXTMENU_H
