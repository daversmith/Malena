// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// EventManager.h
//

#ifndef _EVENTSMANAGER_H
#define _EVENTSMANAGER_H

#include <Malena/Core/malena_export.h>
#include <map>
#include <string>
#include <vector>
#include <Malena/Engine/Events/Callback.h>
#include <Malena/Core/DeferredOperationsManager.h>
#include <Malena/Utilities/EnumKey.h>
#include <SFML/Window/Event.hpp>
#include <optional>

namespace ml
{
    class Fireable;
    class EventReceiver;
    class Core;
    class Unsubscribable;

    /**
     * @brief Centralized event bus for the Malena trait-based event system.
     * @ingroup EngineEvents
     *
     * Stores subscribers keyed by @c std::string — generated from any enum
     * value via @c EnumKey::get(). Both @c ml::Event built-in values and
     * user manifest event enums work identically.
     *
     * All public methods take enum values directly — @c EnumKey::get() is
     * called internally. Users and framework code never see raw string keys.
     *
     * Inherits @c DeferredOperationsManager — any @c unsubscribe call during
     * @c fire() is deferred until iteration ends, preventing iterator
     * invalidation and use-after-free crashes.
     *
     * @see Fireable, EventReceiver, EnumKey, DeferredOperationsManager
     */
    class MALENA_API EventManager : public DeferredOperationsManager<EventManager>
    {
        /// @cond INTERNAL
        struct Subscriber
        {
            EventReceiver* receiver;  ///< trait subobject — for process()
            Core*          core;      ///< Core* — for filter(); nullptr if not Core
        };

        inline static std::map<
            std::string,
            std::vector<Subscriber>
        > _subscribers;
        /// @endcond

    public:
        /**
         * @brief Register a component for an enum-keyed event.
         *
         * @tparam EnumType Any enum type.
         * @param  eventEnum The enum value identifying the event.
         * @param  component The trait @c EventReceiver* subobject.
         */
        template<typename EnumType>
        static void subscribe(EnumType eventEnum, EventReceiver* component)
        {
            doSubscribe(EnumKey::get(eventEnum), component);
        }

        /**
         * @brief Remove a component's subscription to one event.
         *
         * Deferred if called during @c fire().
         *
         * @tparam EnumType Any enum type.
         * @param  eventEnum The event to unsubscribe from.
         * @param  core      The component's @c Core* identity.
         */
        template<typename EnumType>
        static void unsubscribe(EnumType eventEnum, Core* core)
        {
            deferOrExecute([key = EnumKey::get(eventEnum), core]()
            {
                doUnsubscribe(key, core);
            });
        }

        /**
         * @brief Remove all subscriptions for a component.
         *
         * Called by @c Core::~Core() and @c Unsubscribable::unsubscribeAll().
         * Deferred if called during @c fire().
         *
         * @param core The component's @c Core* identity.
         */
        static void unsubscribeAll(Core* core);

        /**
         * @brief Remove all subscriptions for all events.
         *
         * Deferred if called during @c fire().
         */
        static void clear();

        /**
         * @brief Fire an event to all matching subscribers.
         *
         * @tparam EnumType Any enum type.
         * @param  eventEnum  The enum value identifying the event.
         * @param  dispatcher The @c Fireable singleton owning filter logic.
         * @param  event      SFML event forwarded to callbacks.
         * @param  resolve    Called per-component after @c process() passes filter.
         * @param  reject     Called per-component when filter fails.
         */
        template<typename EnumType>
        static void fire(EnumType eventEnum,
                         Fireable* dispatcher,
                         const std::optional<sf::Event>& event,
                         SystemCallback resolve = nullptr,
                         SystemCallback reject  = nullptr)
        {
            doFire(EnumKey::get(eventEnum), dispatcher, event, resolve, reject);
        }

        friend class AppManager;
        friend class Fireable;
        friend class Core;
        friend class Unsubscribable;

    private:
        /// @cond INTERNAL
        static void unsubscribe(const std::string& key, Core* core)
        {
            deferOrExecute([key, core]() { doUnsubscribe(key, core); });
        }



        static void doSubscribe(const std::string& key, EventReceiver* component);
        static void doFire(const std::string& key,
                           Fireable* dispatcher,
                           const std::optional<sf::Event>& event,
                           SystemCallback resolve,
                           SystemCallback reject);
        static void doUnsubscribe(const std::string& key, Core* core);
        static void doUnsubscribeAll(Core* core);

    public:
    	static void forceUnsubscribeAll(Core* core);
        /// @endcond
    };

} // namespace ml

#endif // _EVENTSMANAGER_H
