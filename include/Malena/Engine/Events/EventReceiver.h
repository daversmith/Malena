//
// EventReceiver.h
//

#ifndef EVENTRECEIVER_H
#define EVENTRECEIVER_H

#pragma once

#include <Malena/Engine/Events/Callback.h>
#include <Malena/Engine/Events/Event.h>
#include <SFML/Window/Event.hpp>
#include <map>
#include <string>
#include <vector>

namespace ml
{
    /**
     * @brief Base class for all event-receiving traits.
     * @ingroup EngineEvents
     *
     * @c EventReceiver is the storage layer shared by every trait that handles
     * events (@c Clickable, @c Hoverable, @c Keyable, etc.). It maintains a map
     * of string keys to callback vectors, where each key is produced by
     * @c EnumKey::get() from any @c ml::Event or manifest event enum value.
     *
     * Framework dispatchers call @c process() to invoke stored callbacks.
     * Traits call @c getCallbacks() to register them. User code interacts with
     * @c EventReceiver only indirectly through the trait convenience methods
     * (@c onClick, @c onHover, @c onUpdate, etc.).
     *
     * @see Clickable, Hoverable, Focusable, Keyable, Updatable, Scrollable,
     *      Unsubscribable, EventsManager, EnumKey
     */
    class EventReceiver
    {
    public:
        /**
         * @brief Invoke all callbacks registered for @p key.
         *
         * Called by framework dispatchers. @p key is the string produced by
         * @c EnumKey::get(eventEnum).
         *
         * @param key   String key identifying the event.
         * @param event SFML event forwarded to each callback.
         */
        virtual void process(const std::string& key,
                             const std::optional<sf::Event>& event);

        /**
         * @brief Invoke all callbacks registered for @p eventName.
         *
         * Template overload that accepts any enum value directly, converting
         * to a string key via @c EnumKey::get() internally.
         *
         * @tparam ENUM_TYPE Any enum type.
         * @param  eventName The enum value identifying the event.
         * @param  event     SFML event forwarded to each callback.
         */
        template<typename ENUM_TYPE>
        void process(ENUM_TYPE eventName, const std::optional<sf::Event>& event);

        virtual ~EventReceiver() = default;

        /// @cond INTERNAL
        /**
         * @brief Return the callback vector for @p key, creating it if needed.
         *
         * Used by trait implementations to store callbacks during subscribe calls.
         *
         * @param key @c EnumKey::get(eventEnum)
         */
        std::vector<EventCallback>& getCallbacks(const std::string& key);
        /// @endcond

    private:
        std::map<std::string, std::vector<EventCallback>> _callbacks;
    };

} // namespace ml

#include "../../../../src/Engine/Events/EventReceiver.tpp"
#endif // EVENTRECEIVER_H
