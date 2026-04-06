// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef GRID_H
#define GRID_H

#pragma once

#include <SFML/Graphics.hpp>
#include <Malena/Core/Core.h>
#include <Malena/Traits/Spatial/Positionable.h>
#include <vector>

namespace ml
{
    /**
     * @brief A layout helper that arranges @c Positionable objects in a grid.
     * @ingroup GraphicsLayouts
     *
     * @c Grid is a pure positioning utility — it does not own, draw, or manage
     * the lifecycle of its items. Items are still registered with the application
     * via @c addComponent and receive events normally. @c Grid only controls
     * their positions.
     *
     * When items are added, or when the grid's position, column count, or
     * spacing changes, @c reposition() recalculates each item's world-space
     * position so they flow left-to-right, top-to-bottom in equal-sized cells.
     *
     * @c Grid is declared @c final — it is not intended to be subclassed.
     *
     * ### Usage
     * @code
     * ml::Grid grid({4, 3}); // 4 columns, 3 rows
     * grid.setPosition({50.f, 100.f});
     * grid.setSpacing(8.f);
     *
     * for (auto& tile : tiles)
     * {
     *     addComponent(tile);   // registers with the app for events/drawing
     *     grid.addItem(tile);   // registers with the grid for layout
     * }
     * @endcode
     *
     * @see Positionable, Core
     */
    class Grid final : public Core
    {
        std::vector<Positionable*> _items;

        float        max_height = 0;
        float        max_width  = 0;
        float        spacing    = 10;
        unsigned int row        = 1;
        unsigned int col        = 1;

        /**
         * @brief Recompute and apply positions for all items.
         *
         * Called automatically whenever the layout parameters change
         * (position, row count, spacing) or an item is added.
         */
        void reposition();

    public:
        /**
         * @brief Construct a single-row grid with default spacing.
         */
        Grid();

        /**
         * @brief Construct a grid with a specified column and row count.
         *
         * @param size @c x is the number of columns, @c y is the number of rows.
         */
        Grid(sf::Vector2u size);

        /**
         * @brief Return the current spacing between cells in pixels.
         * @return Spacing in pixels.
         */
        [[nodiscard]] float getSpacing() const;

        /**
         * @brief Set the spacing between cells.
         *
         * Triggers @c reposition() to update all item positions.
         *
         * @param spacing Gap in pixels between adjacent cells.
         */
        void setSpacing(float spacing);

        /**
         * @brief Return the current number of columns.
         * @return Column count.
         */
        [[nodiscard]] int getRow() const;

        /**
         * @brief Set the number of columns per row.
         *
         * Triggers @c reposition() to reflow all items.
         *
         * @param row Number of columns. Items beyond this count wrap to
         *            the next row.
         */
        void setRow(int row);

        /**
         * @brief Add a @c Core object to the grid layout and reposition all items.
         *
         * The item is NOT owned by the grid — register it with the application
         * via @c addComponent separately so it receives events and is drawn.
         *
         * @param item The component to include in the layout.
         */
        void addItem(Core& item);

        /**
         * @brief Set the world-space origin of the grid.
         *
         * Triggers @c reposition() so all items shift relative to the new origin.
         *
         * @param position New top-left position in world coordinates.
         */
        void setPosition(const sf::Vector2f& position) override;

        /**
         * @brief Return the current world-space origin of the grid.
         * @return Top-left position in world coordinates.
         */
        [[nodiscard]] sf::Vector2f getPosition() const override;

        /**
         * @brief Return the axis-aligned bounding box of the entire grid.
         * @return Bounding rectangle in world coordinates.
         */
        [[nodiscard]] sf::FloatRect getGlobalBounds() const override;
    };

} // namespace ml

#endif // GRID_H
