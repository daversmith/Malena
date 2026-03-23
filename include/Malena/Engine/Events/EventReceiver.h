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
#include <Malena/Engine/Events/Event.h>
namespace ml
{
    /**
     * @brief Base class for all event receiver traits.
     * @ingroup Traits
     *
     * Stores callbacks keyed by @c std::string — generated from any enum
     * value via @c EnumKey::get(). Both @c ml::Event built-in values and
     * user manifest event enums use the same storage transparently.
     *
     * Traits call @c getCallbacks() to store callbacks.
     * @c _EventsManager calls @c process() to invoke them.
     *
     * @see Clickable, Hoverable, Focusable, Keyable, Updatable, Scrollable,
     *      Unsubscribable, _EventsManager, EnumKey
     */
    class EventReceiver
    {
    public:
        /**
         * @brief Invoke all callbacks registered for @p key.
         * @param key   @c EnumKey::get(eventEnum)
         * @param event SFML event forwarded to each callback.
         */
        virtual void process(const std::string& key,
                             const std::optional<sf::Event>& event);

    	template <typename ENUM_TYPE>
		void process(ENUM_TYPE eventName, const std::optional<sf::Event>& event);

        /**
         * @brief Return the callback vector for @p key, creating if needed.
         * @param key @c EnumKey::get(eventEnum)
         */
        std::vector<EventCallback>& getCallbacks(const std::string& key);

        virtual ~EventReceiver() = default;

    private:
        std::map<std::string, std::vector<EventCallback>> _callbacks;
    };

} // namespace ml

#include "../../../../src/Engine/Events/EventReceiver.tpp"
#endif // EVENTRECEIVER_H
