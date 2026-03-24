//
// Draggable.h
//

#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#pragma once

#include <Malena/Traits/Base/Customizable.h>
#include <Malena/Engine/Events/EventReceiver.h>
#include <Malena/Engine/Events/Fireable.h>
#include <Malena/Manifests/Manifest.h>
#include <SFML/Graphics.hpp>
#include <optional>

#include "Malena/Engine/Events/EventDispatcher.h"

namespace ml
{
    /**
     * @brief Manifest for the @c Draggable trait.
     * @ingroup Traits
     */
    class DraggableManifest : public Manifest
    {
    public:
        enum class State
        {
            FREE,   ///< Drag freely on both axes (default).
            LOCK_X, ///< Lock horizontal — drag Y axis only.
            LOCK_Y  ///< Lock vertical — drag X axis only.
        };

        enum class Flag
        {
            DRAGGING ///< Set while the user is actively dragging.
        };
    };

    /**
     * @brief Receiver trait that adds mouse-drag behavior to any @c Component.
     * @ingroup Traits
     *
     * Dragging is disabled by default. Enable via:
     * @code
     * myComponent.setFlag(ml::Flag::DRAGGABLE);
     * @endcode
     *
     * ### Axis locking
     * @code
     * myComponent.setState(ml::Draggable::State::LOCK_X); // Y axis only
     * myComponent.setState(ml::Draggable::State::LOCK_Y); // X axis only
     * myComponent.setState(ml::Draggable::State::FREE);   // both axes
     * @endcode
     *
     * ### Drag bounds
     * @code
     * myComponent.setDragBounds(sf::FloatRect{{50.f, 50.f}, {400.f, 300.f}});
     * myComponent.clearDragBounds();
     * @endcode
     *
     * @see DraggableManifest, DraggableDispatcher
     */
    class Draggable : public EventReceiver,
                      public Customizable<DraggableManifest>
    {
    public:
        using Flag  = DraggableManifest::Flag;
        using State = DraggableManifest::State;

        Draggable() = default;
        virtual ~Draggable() = default;

        void setDragBounds(const sf::FloatRect& bounds);
        void clearDragBounds();

    private:
        sf::Vector2f                 _dragOffset;
        std::optional<sf::FloatRect> _dragBounds;

        friend class DraggableDispatcher;
    };

    /**
     * @brief Dispatcher singleton for drag events.
     * @ingroup Traits
     *
     * Fires drag handling for all components that have @c ml::Flag::DRAGGABLE
     * set. Registered automatically via @c ML_EXPORT(DraggableDispatcher).
     */
    class DraggableDispatcher : public EventDispatcher
    {
    public:
        bool occurred(const std::optional<sf::Event>& event) override;
        bool filter(const std::optional<sf::Event>& event, Core* component) override;
        void fire(const std::optional<sf::Event>& event) override;
    };

} // namespace ml

ML_EXPORT(DraggableDispatcher)

#endif // DRAGGABLE_H
