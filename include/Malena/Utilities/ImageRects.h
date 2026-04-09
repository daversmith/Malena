// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 4/9/25.
//

#ifndef IMAGERECTS_H
#define IMAGERECTS_H

#include <SFML/Graphics/Rect.hpp>
#include <vector>

namespace ml
{
    /**
     * @brief A grid-indexed collection of @c sf::IntRect regions.
      * @ingroup Utilities
     *
     * @c ImageRects stores the sub-rectangle regions produced by
     * @c TextureSlicer::getImageRects when a texture is divided into a
     * uniform grid of rows and columns. Each rect describes the pixel
     * bounds of one cell within the source texture, ready to be passed
     * to @c sf::Sprite::setTextureRect for sprite-sheet animation or
     * tile-based rendering.
     *
     * Rects are stored in row-major order and accessed by (row, col) index.
     *
     * ### Usage
     * @code
     * ml::ImageRects rects = ml::TextureSlicer::getImageRects(spriteSheet, 4, 8);
     *
     * // Access frame (row 1, col 3)
     * sprite.setTextureRect(rects.getIntRect(1, 3));
     * @endcode
     *
     * @see TextureSlicer
     */
    class ImageRects
    {
    private:
        std::vector<sf::IntRect> _intRects;
        int _rows;
        int _cols;

        struct RowProxy
        {
            const ImageRects& owner;
            int row;
            const sf::IntRect& operator[](int col) const { return owner.getIntRect(row, col); }
        };

    public:
        /**
         * @brief Construct an empty @c ImageRects with a known column count.
         *
         * @param cols Number of columns in the source grid. Used to convert
         *             (row, col) indices to a flat vector position.
         */
        ImageRects(int cols);

        /**
         * @brief Return the rect for a given row and column.
         *
         * @param row Zero-based row index.
         * @param col Zero-based column index.
         * @return Const reference to the @c sf::IntRect for that cell.
         */
        const sf::IntRect& getIntRect(int row, int col) const;

        /**
         * @brief Row-indexed access — returns a proxy for column subscript.
         *
         * Enables two-dimensional subscript syntax:
         * @code
         * sprite.setTextureRect(rects[1][3]);
         * @endcode
         */
        RowProxy operator[](int row) const { return {*this, row}; }

        /**
         * @brief Append a rect to the collection.
         *
         * Called by @c TextureSlicer to populate the collection in
         * row-major order. Direct use is uncommon outside of framework internals.
         *
         * @param int_rect The rect to append.
         */
        void push(const sf::IntRect& int_rect);
    };

} // namespace ml

#endif // IMAGERECTS_H
