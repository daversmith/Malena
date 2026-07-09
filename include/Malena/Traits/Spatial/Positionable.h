// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

//
// Created by Dave Smith on 11/13/22.
//

#ifndef MALENA_PORTFOLIO_POSITIONABLE_H
#define MALENA_PORTFOLIO_POSITIONABLE_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <SFML/Graphics.hpp>
#include <memory>
#include <Malena/Traits/Base/Trait.h>

namespace ml
{
    class Animate;
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
    public:
        // Positionable stores NO animation member (that would delete the copy
        // assignment the framework relies on for value-type shapes). The Animate
        // controller is owned in an external pointer-keyed registry; this destructor
        // releases it. Declared out-of-line so Animate stays a forward declaration here.
        virtual ~Positionable();

        /**
         * @brief Access this object's animation controller.
         *
         * Lazily created on first use. Drives tweened position (and any
         * app-supplied property) via delta-time @c Tweener / @c Easing.
         *
         * @code
         * comp.animate().move({400.f, 300.f}, 0.4f, ml::Easing::EaseOutCubic);
         * @endcode
         *
         * @return Reference to this object's @c Animate controller.
         */
        Animate& animate();

        /**
         * @brief Set the world-space position immediately (no animation).
         *
         * Pure virtual — implemented by the concrete shape or adapter class.
         *
         * @param position New position in world coordinates.
         */
        virtual void setPosition(const sf::Vector2f& position) {}

        /**
         * @brief Return the current world-space position.
         *
         * @return Current position in world coordinates.
         */
        virtual sf::Vector2f getPosition() const { return {}; }

        /**
         * @brief Return the axis-aligned bounding box in world space.
         *
         * Used by @c AppManager for hit-testing (click, hover) and by
         * layout helpers for relative placement. Returns a zero-size rect
         * by default, opting the component out of event routing.
         *
         * @return Bounding rectangle in world coordinates.
         */
        virtual sf::FloatRect getGlobalBounds() const { return {}; }

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
#endif // MALENA_PORTFOLIO_POSITIONABLE_H
