// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 11/9/25.
//

#ifndef ROUNDEDRECTANGLE_H
#define ROUNDEDRECTANGLE_H

#include <Malena/Core/malena_export.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <Malena/Graphics/Base/Graphic.h>
#include <Malena/Traits/Base/Customizable.h>

namespace ml
{
    /**
     * @brief An @c sf::Shape subclass that draws a rectangle with rounded corners.
      * @ingroup GraphicsPrimitives
     *
     * @c RoundedRectangle extends @c sf::Shape by generating vertex geometry
     * for a rectangle whose corners are replaced by circular arcs. The number
     * of vertices per arc is configurable, trading smoothness for polygon count.
     *
     * It is the underlying shape type used by @c ml::Rectangle — most user
     * code interacts with @c RoundedRectangle indirectly through that class.
     * Direct use is appropriate when you need fine control over corner radius
     * or point count without the additional framework layers.
     *
     * ### Usage
     * @code
     * ml::RoundedRectangle rr({200.f, 80.f}, 12.f, 10);
     * rr.setFillColor(sf::Color::Blue);
     * rr.setPosition({100.f, 100.f});
     * window.draw(rr);
     * @endcode
     *
     * Setting @p radius to @c 0 produces a standard sharp-cornered rectangle.
     *
     * @see Rectangle, Graphic
     */
    class MALENA_API RoundedRectangle : public sf::Shape
    {
    public:
        /**
         * @brief Construct a rounded rectangle.
         *
         * @param size             Width and height of the rectangle in pixels.
         *                         Defaults to @c {0, 0}.
         * @param radius           Corner arc radius in pixels. Defaults to @c 0
         *                         (sharp corners). Clamped to half the shorter
         *                         side so corners never overlap.
         * @param cornerPointCount Number of vertices used to approximate each
         *                         corner arc. Higher values produce smoother
         *                         corners at the cost of more geometry.
         *                         Defaults to @c 8.
         */
        explicit RoundedRectangle(const sf::Vector2f& size = sf::Vector2f(0, 0),
                                  float radius = 0.f,
                                  unsigned int cornerPointCount = 8);

        /**
         * @brief Set the width and height of the rectangle.
         *
         * Triggers geometry regeneration.
         *
         * @param size New size in pixels.
         */
        void setSize(const sf::Vector2f& size);

        /**
         * @brief Return the current size of the rectangle.
         * @return Width and height in pixels.
         */
        const sf::Vector2f& getSize() const;

        /**
         * @brief Set the corner arc radius.
         *
         * Triggers geometry regeneration. Values larger than half the shorter
         * side are clamped automatically.
         *
         * @param radius Corner radius in pixels.
         */
        void setRadius(float radius);

        /**
         * @brief Return the current corner arc radius.
         * @return Corner radius in pixels.
         */
        float getRadius() const;

        /**
         * @brief Set the number of vertices per corner arc.
         *
         * Triggers geometry regeneration.
         *
         * @param count Vertex count per corner. Must be at least @c 1.
         */
        void setCornerPointCount(unsigned int count);

        /**
         * @brief Return the current per-corner vertex count.
         * @return Number of vertices per corner arc.
         */
        unsigned int getCornerPointCount() const;

        /**
         * @brief Return the total number of shape vertices.
         *
         * Equal to @c cornerPointCount × 4 (one arc per corner).
         * Required by @c sf::Shape.
         *
         * @return Total vertex count.
         */
        virtual std::size_t getPointCount() const override;

        /**
         * @brief Return the position of a single vertex.
         *
         * Required by @c sf::Shape for geometry generation.
         *
         * @param index Vertex index in [0, getPointCount()).
         * @return Local-space position of the vertex.
         */
        virtual sf::Vector2f getPoint(std::size_t index) const override;

    private:
        sf::Vector2f m_size;
        float        m_radius;
        unsigned int m_cornerPointCount;
    };

} // namespace ml

#endif // ROUNDEDRECTANGLE_H
