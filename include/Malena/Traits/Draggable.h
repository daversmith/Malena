//
// Created by Dave Smith on 11/3/25.
//

#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#pragma once

#include <SFML/Graphics.hpp>
#include <optional>
#include <Malena/Traits/Customizable.h>
#include <Malena/Manifests/Manifest.h>

namespace ml
{
    /**
     * @brief Manifest for the @c Draggable trait.
     *
     * Declares the @c State and @c Flag enums that @c Draggable needs.
     * These are gathered automatically into any @c ComponentWith that
     * mixes in @c Draggable, so they appear alongside the component's
     * own manifest flags and states in a single overload set.
     */
    class DraggableManifest : public Manifest
    {
    public:
        /// Controls which axes mouse drag is allowed on.
        enum class State
        {
            FREE,   ///< Drag freely on both axes (default).
            LOCK_X, ///< Lock horizontal movement — drag on Y axis only.
            LOCK_Y  ///< Lock vertical movement — drag on X axis only.
        };

        /// Runtime flags owned by the @c Draggable trait.
        enum class Flag
        {
            DRAGGING ///< Set while the user is actively dragging this component.
        };
    };

    /**
     * @brief Trait that adds mouse-drag behavior to any @c Component.
     *
     * @c Draggable is mixed into every @c ComponentCore automatically — it
     * does not need to be listed explicitly as a trait. Dragging is disabled
     * by default and must be enabled via the system @c ml::Flag:
     *
     * @code
     * myComponent.setFlag(ml::Flag::DRAGGABLE);
     * @endcode
     *
     * ### Axis locking
     * Constrain movement to one axis via the @c DraggableManifest::State enum:
     * @code
     * myComponent.setState(ml::Draggable::State::LOCK_X); // Y axis only
     * myComponent.setState(ml::Draggable::State::LOCK_Y); // X axis only
     * myComponent.setState(ml::Draggable::State::FREE);   // both axes (default)
     * @endcode
     *
     * ### Drag bounds
     * Optionally constrain the drag area to a rectangle:
     * @code
     * myComponent.setDragBounds(sf::FloatRect{{50.f, 50.f}, {400.f, 300.f}});
     * myComponent.clearDragBounds(); // remove the constraint
     * @endcode
     *
     * ### How it works internally
     * @c ComponentCore subscribes to the @c "draggable" event in its
     * constructor. Each frame @c UIManager fires that event; the callback
     * checks @c ml::Flag::DRAGGABLE and, if set, delegates to
     * @c handleDragEvent. This means the drag subscription exists on every
     * component but is effectively a no-op until dragging is enabled.
     *
     * @see DraggableManifest, ComponentCore, Positionable
     */
    class Draggable : public Customizable<DraggableManifest>
    {
    public:
        /// Convenience alias — allows @c setState(Draggable::State::LOCK_X).
        using Flag  = DraggableManifest::Flag;

        /// Convenience alias — allows @c setState(Draggable::State::LOCK_X).
        using State = DraggableManifest::State;

        Draggable() = default;
        virtual ~Draggable() = default;

        /**
         * @brief Constrain dragging to a world-space rectangle.
         *
         * While a drag bound is active, @c handleDragEvent clamps the
         * component's position so it cannot be dragged outside @p bounds.
         *
         * @param bounds The allowed drag area in world coordinates.
         */
        void setDragBounds(const sf::FloatRect& bounds);

        /**
         * @brief Remove any previously set drag bounds.
         *
         * After this call the component can be dragged anywhere in the window.
         */
        void clearDragBounds();

        /**
         * @brief Process one drag event tick.
         *
         * Called internally by the @c "draggable" event subscription registered
         * in @c ComponentCore. Reads the SFML mouse state, updates the drag
         * offset, respects axis locks and drag bounds, and calls
         * @c setPosition accordingly.
         *
         * @note This method is part of the framework's internal event plumbing.
         *       User code should interact with @c Draggable through
         *       @c setFlag(ml::Flag::DRAGGABLE), @c setState, and
         *       @c setDragBounds rather than calling this directly.
         *
         * @param event The current SFML event, may be empty.
         */
        void handleDragEvent(const std::optional<sf::Event>& event);

    private:
        sf::Vector2f                 _dragOffset;  ///< Offset from component origin to mouse at drag start.
        std::optional<sf::FloatRect> _dragBounds;  ///< Optional world-space drag constraint.
    };

} // namespace ml

#endif // DRAGGABLE_H
