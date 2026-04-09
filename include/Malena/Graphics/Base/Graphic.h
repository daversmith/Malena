// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef GRAPHIC_H
#define GRAPHIC_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <SFML/Graphics/RenderTarget.hpp>
#include <Malena/Core/Component.h>
#include <Malena/Resources/FontManager.h>

namespace ml
{
    /**
     * @brief Base template for all Malena graphical primitives.
     *
     * @c Graphic<T> composes an SFML drawable type @c T (e.g.,
     * @c sf::RectangleShape, @c sf::CircleShape, @c sf::Sprite) with
     * @c ComponentCore<>, giving the resulting class the full framework
     * trait set — @c Subscribable, @c Flaggable, @c Positionable,
     * @c Draggable, and custom flags/states — without introducing a
     * second @c sf::Drawable base and the diamond it would cause.
     *
     * ### Why not @c ComponentBase?
     * @c ComponentBase adds @c sf::Drawable @e and registers empty
     * @c onClick / @c onHover subscriptions. @c Graphic deliberately
     * inherits @c ComponentCore instead, because the SFML shape @c T
     * already @e is the drawable, and internal shape members must @b not
     * intercept click/hover events — only the outermost @c Component
     * wrapper (e.g., @c Rectangle, @c Circle) should participate in
     * hit-testing.
     *
     * ### Forwarding constructors
     * @c T::T is brought in so that any SFML constructor that the shape
     * type supports can be used directly:
     * @code
     * ml::Circle c(50.f); // forwards to sf::CircleShape(float radius)
     * @endcode
     *
     * ### Typical use
     * @c Graphic is not instantiated directly. Use the concrete subclasses:
     * @code
     * ml::Rectangle rect;
     * ml::Circle    circle;
     * ml::Sprite    sprite;
     * @endcode
     *
     * @tparam T An SFML drawable type (sf::Shape subclass, sf::Sprite, etc.)
     *           that provides @c setPosition, @c getPosition, and
     *           @c getGlobalBounds.
     *
     * @ingroup GraphicsBase
     * @see Rectangle, Circle, Sprite, ComponentCore, Component
     */
    template<typename T>
    class Graphic : public T, public virtual ComponentCore<>
    {
    public:
        using T::T;

        /**
         * @brief Implicit conversion to a const @c sf::Drawable reference.
         *
         * Allows a @c Graphic to be passed wherever a raw @c sf::Drawable&
         * is expected (e.g., @c window.draw()).
         *
         * @return Const reference to this object as @c sf::Drawable.
         */
        operator const sf::Drawable&() const;

        /// @cond INTERNAL
        static T isItText();
        /// @endcond

        Graphic();

        /**
         * @brief Construct from an existing SFML object of type @c U.
         *
         * @tparam U Source type, must be compatible with @c T's copy/move constructor.
         * @param  obj The source object to copy-construct from.
         */
        template<typename U>
        explicit Graphic(const U& obj);

        /**
         * @brief Draw this graphic to an SFML render target.
         *
         * Delegates to @c T::draw after applying any render states returned
         * by @c getRenderStates() (e.g., a shader set on this component).
         *
         * @param target SFML render target (window or texture).
         * @param states Current render states (transform, blend mode, etc.).
         */
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        /**
         * @brief Return the axis-aligned bounding box in world space.
         *
         * Delegates to @c T::getGlobalBounds(). Used by @c UIManager for
         * hit-testing (click, hover) and by layout helpers.
         *
         * @return Bounding rectangle in world coordinates.
         */
        sf::FloatRect getGlobalBounds() const override;

        /**
         * @brief Set the world-space position.
         *
         * Delegates to @c T::setPosition.
         *
         * @param position New position in world coordinates.
         */
        void setPosition(const sf::Vector2f& position) override;

        /**
         * @brief Return the current world-space position.
         *
         * Delegates to @c T::getPosition.
         *
         * @return Current position in world coordinates.
         */
        sf::Vector2f getPosition() const override;
    };

} // namespace ml

#include "../../../../src/Graphics/Base/Graphic.tpp"
#endif // SHAPE_H
