// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 4/9/25.
//

#ifndef TEXTURESLICER_H
#define TEXTURESLICER_H

#include <SFML/Graphics.hpp>
#include <Malena/Utilities/ImageRects.h>

namespace ml
{
    /**
     * @brief Divides a texture into a uniform grid of sub-rectangles.
      * @ingroup Utilities
     *
     * @c TextureSlicer calculates the pixel bounds of each cell when a
     * texture is divided into @p rows rows and @p cols columns of equal
     * size, returning the results as an @c ImageRects collection.
     *
     * This is the primary utility for working with sprite sheets and
     * tile atlases — slice once, then use the resulting rects with
     * @c sf::Sprite::setTextureRect to display individual frames or tiles.
     *
     * ### Usage
     * @code
     * // A sprite sheet with 4 rows and 8 columns of frames
     * const sf::Texture& sheet = ml::TextureManager<MyManifest>::get(MyManifest::Images::Player);
     * ml::ImageRects rects = ml::TextureSlicer::getImageRects(sheet, 4, 8);
     *
     * // Display frame at row 0, column 2
     * sprite.setTexture(sheet);
     * sprite.setTextureRect(rects.getIntRect(0, 2));
     * @endcode
     *
     * @see ImageRects, TextureManager
     */
    class TextureSlicer
    {
    public:
        /**
         * @brief Slice a texture into a rows × cols grid of equal rectangles.
         *
         * Divides the texture's pixel dimensions evenly, computing one
         * @c sf::IntRect per cell in row-major order. The rects are stored
         * in the returned @c ImageRects object.
         *
         * @param texture The source texture to slice. Must be fully loaded.
         * @param rows    Number of rows to divide the texture into.
         * @param cols    Number of columns to divide the texture into.
         * @return An @c ImageRects containing @c rows × @c cols rects,
         *         accessible by (row, col) index.
         */
        static ml::ImageRects getImageRects(const sf::Texture& texture, int rows, int cols);
    };

} // namespace ml

#endif // TEXTURESLICER_H
