#ifndef GRID_H
#define GRID_H

#pragma once

#include <SFML/Graphics.hpp>
#include <Malena/Engine/App/Application.h>
#include <Malena/Core/CoreManager.h>

namespace ml
{
    /**
     * @brief A layout container that arranges @c Positionable objects in a grid.
     *
     * @c Grid acts as both a @c CoreManager<Positionable> (it owns and manages
     * a collection of positioned objects) and a @c Core (it has its own
     * position and bounds, so it can be placed in the scene like any other
     * framework object).
     *
     * When components are added via @c addComponent, or when the grid's
     * position, row count, or spacing changes, @c reposition() recalculates
     * each component's world-space position so items flow left-to-right,
     * top-to-bottom in equal-sized cells.
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
     *     grid.addComponent(tile);
     *
     * // Grid itself does not need to be added to the app —
     * // its managed components are positioned directly.
     * @endcode
     *
     * ### Layout model
     * Items are placed in row-major order. The cell size is determined by
     * the largest item dimensions seen so far (tracked via @c max_width
     * and @c max_height). @c spacing is added between every cell.
     *
     * @see CoreManager, Core, Positionable
     */
    class Grid final : public CoreManager<Positionable>, public Core
    {
        float        max_height = 0;
        float        max_width  = 0;
        float        spacing    = 10;
        unsigned int row        = 1;
        unsigned int col        = 1;

        /**
         * @brief Recompute and apply positions for all managed components.
         *
         * Called automatically whenever the layout parameters change
         * (position, row count, spacing) or a component is added.
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
         * Triggers @c reposition() to update all component positions.
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
         * Triggers @c reposition() to reflow all managed components.
         *
         * @param row Number of columns. Items beyond this count wrap to
         *            the next row.
         */
        void setRow(int row);

        /**
         * @brief Add a @c Core object to the grid and reposition all items.
         *
         * The item is registered with the underlying @c CoreManager and its
         * position is set according to the current grid layout.
         *
         * @note Do not copy or move a component after it has been added —
         *       the grid stores a raw pointer to it.
         *
         * @param component The component to add.
         */
        void addComponent(Core& component);

        /**
         * @brief Set the world-space origin of the grid.
         *
         * Triggers @c reposition() so all managed components shift
         * relative to the new origin.
         *
         * @param position New top-left position in world coordinates.
         */
        void setPosition(const sf::Vector2f& position);

        /**
         * @brief Return the current world-space origin of the grid.
         * @return Top-left position in world coordinates.
         */
        [[nodiscard]] sf::Vector2f getPosition() const;

        /**
         * @brief Return the axis-aligned bounding box of the entire grid.
         *
         * Encompasses all managed components from the top-left origin to
         * the bottom-right of the last cell (including spacing).
         *
         * @return Bounding rectangle in world coordinates.
         */
        [[nodiscard]] sf::FloatRect getGlobalBounds() const;
    };

} // namespace ml

#endif // GRID_H
