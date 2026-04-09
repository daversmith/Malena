// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 11/13/22.
//

#ifndef PORTFOLIO_POSITIONABLE_H
#define PORTFOLIO_POSITIONABLE_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <SFML/Graphics.hpp>
#include <float.h>
#include <queue>
#include <Malena/Traits/Base/Trait.h>
#include <Malena/Utilities/Tween.h>

namespace ml
{
    /**
     * @brief Trait that provides position, bounds, and animated movement.
      * @ingroup Traits
     *
     * @c Positionable is one of the three core traits on every @c ml::Core
     * object. It serves two purposes:
     *
     * 1. **Spatial contract** — declares the pure virtual @c setPosition,
     *    @c getPosition, and @c getGlobalBounds methods that the rest of the
     *    framework (hit-testing, layout, drag) depend on.
     *
     * 2. **Animated movement** — provides @c moveTo and @c moveDistance for
     *    tweened position animation, with support for axis locking, easing
     *    curves, and a configurable framerate target.
     *
     * ### Immediate positioning
     * @code
     * // Implemented by the concrete shape/component class
     * myRect.setPosition({100.f, 200.f});
     * sf::Vector2f pos = myRect.getPosition();
     * sf::FloatRect bounds = myRect.getGlobalBounds();
     * @endcode
     *
     * ### Animated movement
     * @code
     * // Animate to an absolute position over 0.5 seconds
     * myRect.moveTo({400.f, 300.f}, 0.5f);
     *
     * // Animate by a relative offset over 1 second (default)
     * myRect.moveDistance({50.f, 0.f});
     *
     * // Poll whether animation is still running
     * if (myRect.isScrolling()) { ... }
     * @endcode
     *
     * ### Relative layout helpers
     * @code
     * labelB.setRightOf(labelA, 10.f);   // place B 10px to the right of A
     * button.setBelow(header, 20.f);     // place button 20px below header
     * icon.center(panel);                // center icon inside panel
     * icon.centerHorizonally(panel);     // center horizontally only
     * @endcode
     *
     * @see Draggable, CoreAdapter, Component
     */
    class MALENA_API Positionable : public Trait
    {
    private:
        float velocityX{}, velocityY{}, framerate = 60.f;
        sf::Vector2f initialPosition = {FLT_MIN, FLT_MAX};
        sf::Clock clock;
        std::queue<sf::Vector2f> points;
        bool scrolling = false;

        bool error(sf::Vector2f a, sf::Vector2f b, float err = 0.1f);
        void generatePoints(sf::Vector2f position, float duration, Tween tween);
        void generateExponential(sf::Vector2f position, float duration);
        void calcVelocity(sf::Vector2f velocity, float seconds);

    public:
        /**
         * @brief Set the assumed framerate for movement interpolation.
         *
         * @c moveTo generates a queue of intermediate positions based on
         * this value. Defaults to 60 fps. Call this if your application
         * runs at a different target rate to keep animation timing accurate.
         *
         * @param framerate Target framerate in frames per second.
         */
        void setFramerate(float framerate);

        /**
         * @brief Animate to an absolute world-space position.
         *
         * Populates an internal waypoint queue and advances through it
         * on successive @c onUpdate ticks. The object's position at the
         * start of the call is the implicit start point.
         *
         * @param position  Target position in world coordinates.
         * @param seconds   Duration of the animation. Defaults to 1.0 s.
         */
        void moveTo(sf::Vector2f position, float seconds = 1.f);

        /**
         * @brief Animate to a position defined by a @c FloatRect origin.
         *
         * Convenience overload that uses the top-left of @p position as
         * the target. Useful when positioning relative to layout rects.
         *
         * @param position  Rect whose origin is the animation target.
         * @param seconds   Duration of the animation. Defaults to 1.0 s.
         */
        void moveTo(sf::FloatRect position, float seconds = 1.f);

        /**
         * @brief Animate by a relative offset from the current position.
         *
         * Equivalent to @c moveTo(getPosition() + distance, seconds).
         *
         * @param distance  Offset in world coordinates to move by.
         * @param seconds   Duration of the animation. Defaults to 1.0 s.
         */
        void moveDistance(sf::Vector2f distance, float seconds = 1.f);

        /**
         * @brief Return @c true while a @c moveTo or @c moveDistance
         *        animation is in progress.
         *
         * @return @c true if the waypoint queue is non-empty.
         */
        bool isScrolling();

        /**
         * @brief Set the world-space position immediately (no animation).
         *
         * Pure virtual — implemented by the concrete shape or adapter class.
         *
         * @param position New position in world coordinates.
         */
        virtual void setPosition(const sf::Vector2f& position) = 0;

        /**
         * @brief Return the current world-space position.
         *
         * Pure virtual — implemented by the concrete shape or adapter class.
         *
         * @return Current position in world coordinates.
         */
        virtual sf::Vector2f getPosition() const = 0;

        /**
         * @brief Return the axis-aligned bounding box in world space.
         *
         * Used by @c UIManager for hit-testing (click, hover) and by
         * layout helpers for relative placement. Pure virtual — implemented
         * by the concrete shape or adapter class.
         *
         * @return Bounding rectangle in world coordinates.
         */
        virtual sf::FloatRect getGlobalBounds() const = 0;

        // ── Relative layout helpers ──────────────────────────────────────────

        /**
         * @brief Position this object immediately to the right of @p obj.
         * @tparam T  Any type with @c getGlobalBounds().
         * @param obj     Reference object.
         * @param spacing Gap in pixels between the two objects. Defaults to 0.
         */
        template<class T>
        void setRightOf(T& obj, float spacing = 0);

        /**
         * @brief Position this object immediately to the left of @p obj.
         * @tparam T  Any type with @c getGlobalBounds().
         * @param obj     Reference object.
         * @param spacing Gap in pixels between the two objects. Defaults to 0.
         */
        template<class T>
        void setLeftOf(T& obj, float spacing = 0);

        /**
         * @brief Position this object immediately below @p obj.
         * @tparam T  Any type with @c getGlobalBounds().
         * @param obj     Reference object.
         * @param spacing Gap in pixels between the two objects. Defaults to 0.
         */
        template<class T>
        void setBelow(T& obj, float spacing = 0);

        /**
         * @brief Position this object immediately above @p obj.
         * @tparam T  Any type with @c getGlobalBounds().
         * @param obj     Reference object.
         * @param spacing Gap in pixels between the two objects. Defaults to 0.
         */
        template<class T>
        void setAbove(T& obj, float spacing = 0);

        /**
         * @brief Center this object within @p obj (both axes).
         * @tparam T  Any type with @c getGlobalBounds().
         * @param obj Container to center within.
         */
        template<class T>
        void center(T& obj);

        /**
         * @brief Center this object horizontally within @p obj.
         * @tparam T  Any type with @c getGlobalBounds().
         * @param obj Container to center within.
         */
        template<class T>
        void centerHorizonally(T& obj);

        /**
         * @brief Center this object vertically within @p obj.
         * @tparam T  Any type with @c getGlobalBounds().
         * @param obj Container to center within.
         */
        template<class T>
        void centerVertically(T& obj);

        /**
         * @brief Center an @c sf::Text object within this object's bounds.
         *
         * Convenience overload for the common pattern of centering a text
         * label inside a button or panel.
         *
         * @param obj The @c sf::Text to reposition.
         */
        void centerText(sf::Text& obj);
    };

} // namespace ml

#include "../../../../src/Traits/Spatial/Positionable.tpp"
#endif // PORTFOLIO_POSITIONABLE_H
