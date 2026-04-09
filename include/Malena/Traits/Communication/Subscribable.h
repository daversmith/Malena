// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef SUBSCRIBABLE_H
#define SUBSCRIBABLE_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Engine/Events/EventManager.h>
#include <Malena/Engine/Events/EventDispatcher.h>
#include <Malena/Engine/Events/EventReceiver.h>
#include <Malena/Engine/Events/Callback.h>
#include <optional>
namespace ml
{
    /**
     * @brief Trait that allows a component to subscribe to and publish framework events.
     * @ingroup Traits
     *
     * @c Subscribable is one of the three core traits automatically included in
     * every @c ml::Core object (@c Subscribable, @c Flaggable, @c Positionable).
     * It is the low-level event subscription layer — most user code will interact
     * with the higher-level trait convenience methods (@c onClick, @c onHover,
     * @c onUpdate, etc.) rather than calling @c subscribe and @c publish directly.
     *
     * Both @c ml::Event built-in values and user-defined manifest event enums
     * work identically — any enum value is accepted.
     *
     * ### Subscribing to a built-in event
     * @code
     * // Preferred — use the trait convenience methods
     * myRect.onClick([]{ std::cout << "clicked\n"; });
     * myRect.onUpdate([]{ /* per-frame logic *\/ });
     *
     * // Advanced — subscribe with a raw ml::Event value
     * myRect.subscribe(ml::Event::CLICK, [](const std::optional<sf::Event>& e){
     *     // handle click with SFML event data
     * });
     * @endcode
     *
     * ### Publishing a custom event
     * @code
     * enum class MyEvent { GameStarted, ScoreChanged };
     *
     * // Publish to all subscribers of MyEvent::GameStarted
     * myRect.publish(MyEvent::GameStarted);
     *
     * // Publish with a filter — only deliver to components within a rect
     * myRect.publish(MyEvent::ScoreChanged,
     *     [&](ml::EventReceiver& r){
     *         auto* c = dynamic_cast<ml::Core*>(&r);
     *         return c && zone.contains(c->getPosition());
     *     });
     * @endcode
     *
     * @see Clickable, Hoverable, Updatable, Unsubscribable, EventManager
     */
    class MALENA_API Subscribable : public EventReceiver
    {
    public:
        /**
         * @brief Subscribe to an enum-keyed event with a full SFML event callback.
         *
         * The callback is invoked each time the event fires and the framework's
         * filter (defined by the associated @c EventDispatcher) passes this
         * component. The @c std::optional<sf::Event> parameter carries the raw
         * SFML event when one is available, or @c std::nullopt for frame events.
         *
         * @tparam ENUM_TYPE Any enum type — @c ml::Event for built-in events,
         *                   or a manifest event enum for custom events.
         * @param  event     The enum value identifying the event to subscribe to.
         * @param  callback  Function invoked with the SFML event when the event fires.
         */
        template<typename ENUM_TYPE>
        void subscribe(ENUM_TYPE event, EventCallback callback);

        /**
         * @brief Subscribe to an enum-keyed event with a no-argument callback.
         *
         * Convenience overload for callbacks that do not need access to the
         * raw SFML event.
         *
         * @tparam ENUM_TYPE Any enum type.
         * @param  event     The enum value identifying the event to subscribe to.
         * @param  callback  Function invoked with no arguments when the event fires.
         */
        template<typename ENUM_TYPE>
        void subscribe(ENUM_TYPE event, Callback callback);

        /**
         * @brief Fire an enum-keyed event to all matching subscribers.
         *
         * Iterates all subscribers registered for @p event and invokes their
         * callbacks for each subscriber that passes the optional @p filter.
         * Optional @p resolve and @p reject system callbacks are called after
         * each component passes or fails the filter respectively.
         *
         * @tparam ENUM_TYPE  Any enum type.
         * @param  event      The enum value identifying the event to fire.
         * @param  filter     Optional predicate; only subscribers for which
         *                    this returns @c true receive the event. Defaults
         *                    to accepting all subscribers.
         * @param  resolve    Optional callback invoked for each subscriber that
         *                    passes the filter.
         * @param  reject     Optional callback invoked for each subscriber that
         *                    fails the filter.
         */
        template<typename ENUM_TYPE>
        void publish(ENUM_TYPE event,
                     FilterCallback filter = [](EventReceiver&){ return true; },
                     SystemCallback resolve = nullptr,
                     SystemCallback reject  = nullptr);
    };

    /// @cond INTERNAL
    class MALENA_API SubscribableDispatcher : public EventDispatcher
    {
    public:
        void fire(const std::optional<sf::Event>& event) override {};
        bool occurred(const std::optional<sf::Event>& event) override {return false;};
    };
    /// @endcond

} // namespace ml
#include "../../../../src/Traits/Communication/Subscribable.tpp"

ML_EXPORT(SubscribableDispatcher)
#endif // MESSAGEMANAGER_H
