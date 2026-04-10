// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 11/13/22.
//

#ifndef MALENA_ALIGN_H
#define MALENA_ALIGN_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <float.h>
#include <queue>
#include "../Utilities/Helper.h"
#include "../Utilities/Tween.h"

namespace ml
{
    /**
     * @brief Static layout helpers for positioning objects relative to one another.
      * @ingroup Utilities
     *
     * @c Align provides a uniform set of spatial operations that work against
     * three kinds of reference:
     *
     * - **Framework object** @c T — any type with @c getGlobalBounds()
     * - **@c sf::FloatRect** — a raw bounding rectangle
     * - **@c sf::Vector2<U>** — treated as a size (width × height) anchored at the origin
     *
     * Each operation calls @c setPosition on the target object @c S, so @c S
     * must provide @c setPosition(sf::Vector2f) and @c getGlobalBounds().
     *
     * These are the static equivalents of the instance methods on @c Positionable.
     * Use @c Align when the reference object is not a @c Positionable (e.g., a
     * raw @c sf::FloatRect from a window size or a texture region).
     *
     * ### Usage
     * @code
     * // Place labelB to the right of labelA with 10 px spacing
     * ml::Align::setRightOf(labelA, labelB, 10.f);
     *
     * // Center a button within a panel's bounds
     * ml::Align::centerOn(panel.getGlobalBounds(), button);
     *
     * // Center text inside a shape
     * ml::Align::centerText(panel, myText);
     *
     * // Center horizontally within a known size
     * ml::Align::centerHorizontally(sf::Vector2f{800.f, 600.f}, mySprite);
     * @endcode
     *
     * @see Positionable
     */
    class MALENA_API Align
    {
    public:
        // ── Object overloads ─────────────────────────────────────────────────

        /**
         * @brief Center @p text within the bounds of @p obj.
         * @tparam T  Reference type with @c getGlobalBounds().
         * @param obj  Reference object.
         * @param text The @c sf::Text to reposition.
         */
        template<class T>
        static void centerText(const T& obj, sf::Text& text);

        /**
         * @brief Place @p obj immediately to the right of @p constObj.
         * @tparam T Reference type. @tparam S Target type.
         * @param constObj Reference object.
         * @param obj      Object to reposition.
         * @param spacing  Gap in pixels between the two objects.
         */
        template<class T, class S>
        static void setRightOf(const T& constObj, S& obj, float spacing = 0);

        /**
         * @brief Place @p obj immediately to the left of @p constObj.
         * @tparam T Reference type. @tparam S Target type.
         * @param constObj Reference object.
         * @param obj      Object to reposition.
         * @param spacing  Gap in pixels between the two objects.
         */
        template<class T, class S>
        static void setLeftOf(const T& constObj, S& obj, float spacing = 0);

        /**
         * @brief Place @p obj immediately below @p constObj.
         * @tparam T Reference type. @tparam S Target type.
         * @param constObj Reference object.
         * @param obj      Object to reposition.
         * @param spacing  Gap in pixels between the two objects.
         */
        template<class T, class S>
        static void setBelow(const T& constObj, S& obj, float spacing = 0);

        /**
         * @brief Place @p obj immediately above @p constObj.
         * @tparam T Reference type. @tparam S Target type.
         * @param constObj Reference object.
         * @param obj      Object to reposition.
         * @param spacing  Gap in pixels between the two objects.
         */
        template<class T, class S>
        static void setAbove(const T& constObj, S& obj, float spacing = 0);

        /**
         * @brief Center @p obj both horizontally and vertically within @p constObj.
         * @tparam T Reference type. @tparam S Target type.
         */
        template<class T, class S>
        static void centerOn(const T& constObj, S& obj);

        /**
         * @brief Center @p obj vertically within @p constObj (horizontal position unchanged).
         * @tparam T Reference type. @tparam S Target type.
         */
        template<class T, class S>
        static void centerVertically(const T& constObj, S& obj);

        /**
         * @brief Center @p obj horizontally within @p constObj (vertical position unchanged).
         * @tparam T Reference type. @tparam S Target type.
         */
        template<class T, class S>
        static void centerHorizontally(const T& constObj, S& obj);

        // ── FloatRect overloads ───────────────────────────────────────────────

        /** @brief Place @p obj to the right of @p bounds. @see setRightOf(const T&, S&, float) */
        template<class S>
        static void setRightOf(const sf::FloatRect& bounds, S& obj, float spacing = 0);

        /** @brief Place @p obj to the left of @p bounds. */
        template<class S>
        static void setLeftOf(const sf::FloatRect& bounds, S& obj, float spacing = 0);

        /** @brief Place @p obj below @p bounds. */
        template<class S>
        static void setBelow(const sf::FloatRect& bounds, S& obj, float spacing = 0);

        /** @brief Place @p obj above @p bounds. */
        template<class S>
        static void setAbove(const sf::FloatRect& bounds, S& obj, float spacing = 0);

        /** @brief Center @p obj within @p bounds (both axes). */
        template<class S>
        static void centerOn(const sf::FloatRect& bounds, S& obj);

        /** @brief Center @p obj vertically within @p bounds. */
        template<class S>
        static void centerVertically(const sf::FloatRect& bounds, S& obj);

        /** @brief Center @p obj horizontally within @p bounds. */
        template<class S>
        static void centerHorizontally(const sf::FloatRect& bounds, S& obj);

        // ── Vector2 overloads (size-based) ────────────────────────────────────

        /**
         * @brief Place @p obj to the right of a region defined by @p size.
         *
         * The region is treated as anchored at the origin with the given size,
         * i.e., equivalent to @c sf::FloatRect{{0,0}, size}.
         *
         * @tparam S Target type. @tparam U Vector component type (e.g., @c float, @c unsigned).
         */
        template<class S, class U>
        static void setRightOf(const sf::Vector2<U>& size, S& obj, float spacing = 0);

        /** @brief Place @p obj to the left of a size-defined region. */
        template<class S, class U>
        static void setLeftOf(const sf::Vector2<U>& size, S& obj, float spacing = 0);

        /** @brief Place @p obj below a size-defined region. */
        template<class S, class U>
        static void setBelow(const sf::Vector2<U>& size, S& obj, float spacing = 0);

        /** @brief Place @p obj above a size-defined region. */
        template<class S, class U>
        static void setAbove(const sf::Vector2<U>& size, S& obj, float spacing = 0);

        /** @brief Center @p obj within a size-defined region (both axes). */
        template<class S, class U>
        static void centerOn(const sf::Vector2<U>& size, S& obj);

        /** @brief Center @p obj vertically within a size-defined region. */
        template<class S, class U>
        static void centerVertically(const sf::Vector2<U>& size, S& obj);

        /** @brief Center @p obj horizontally within a size-defined region. */
        template<class S, class U>
        static void centerHorizontally(const sf::Vector2<U>& size, S& obj);
    };

} // namespace ml

#include "../../../src/Utilities/Align.tpp"
#endif // MALENA_ALIGN_H
