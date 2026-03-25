// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave R. Smith on 3/3/25.
//

#ifndef MOUSEEVENTS_H
#define MOUSEEVENTS_H

#pragma once

#include <SFML/Graphics.hpp>

namespace ml
{
    /**
     * @brief Static helpers for mouse hit-testing against framework objects.
      * @ingroup Utilities
     *
     * @c MouseEvents provides the low-level spatial queries that @c UIManager
     * uses each frame to determine which component the mouse is hovering over
     * or clicking on. Each method is overloaded to accept either a framework
     * object (queried via @c getGlobalBounds()) or a raw @c sf::FloatRect.
     *
     * Both @c isHovered and @c isClicked convert the current mouse position
     * from screen space to world space before testing, so they work correctly
     * with any view transform.
     *
     * @note User code rarely calls these directly. The preferred approach is
     *       to use the @c Subscribable event callbacks (@c onClick, @c onHover)
     *       which are fired automatically by @c UIManager.
     *
     * @see UIManager, Subscribable, Positionable
     */
    class MouseEvents
    {
    public:
        /**
         * @brief Return @c true if the mouse is currently over a framework object.
         *
         * Delegates to @c isHovered(obj.getGlobalBounds(), window).
         *
         * @tparam T  Any type with a @c getGlobalBounds() method returning
         *            @c sf::FloatRect.
         * @param obj    The object to test.
         * @param window The render window used to map mouse coordinates to
         *               world space.
         * @return @c true if the mouse position is inside @c obj's bounds.
         */
        template<typename T>
        static bool isHovered(const T& obj, const sf::RenderWindow& window)
        {
            return isHovered(obj.getGlobalBounds(), window);
        }

        /**
         * @brief Return @c true if the mouse is currently over a rect.
         *
         * Maps @c sf::Mouse::getPosition(window) to world space via the
         * window's current view, then tests containment against @p bounds.
         *
         * @param bounds The rectangle to test in world coordinates.
         * @param window The render window providing the view transform.
         * @return @c true if the mapped mouse position is inside @p bounds.
         */
        static bool isHovered(const sf::FloatRect& bounds, const sf::RenderWindow& window);

        /**
         * @brief Return @c true if the left mouse button is held over a framework object.
         *
         * Delegates to @c isClicked(obj.getGlobalBounds(), window).
         *
         * @tparam T  Any type with a @c getGlobalBounds() method returning
         *            @c sf::FloatRect.
         * @param obj    The object to test.
         * @param window The render window used to map mouse coordinates.
         * @return @c true if the left button is down and the mouse is inside
         *         @c obj's bounds.
         */
        template<typename T>
        static bool isClicked(const T& obj, const sf::RenderWindow& window)
        {
            return isClicked(obj.getGlobalBounds(), window);
        }

        /**
         * @brief Return @c true if the left mouse button is held over a rect.
         *
         * Combines @c sf::Mouse::isButtonPressed(Left) with a hover test
         * against @p bounds.
         *
         * @param bounds The rectangle to test in world coordinates.
         * @param window The render window providing the view transform.
         * @return @c true if the left button is down and the mouse is inside
         *         @p bounds.
         */
        static bool isClicked(const sf::FloatRect& bounds, const sf::RenderWindow& window);
    };

} // namespace ml

#endif // MOUSEEVENTS_H
