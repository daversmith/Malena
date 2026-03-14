//
// EventsManager.cpp
//

#include <functional>
#include <iostream>
#include <optional>
#include <Malena/Engine/Events/EventsManager.h>

namespace ml
{
    std::optional<std::map<Subscribable *, std::function<void(const std::optional<sf::Event> &)>> *>
    EventsManager::getEvent(const std::string &event)
    {
        auto it = events.find(event);
        if (it != events.end())
        {
            return &it->second;
        }
        return std::nullopt;
    }

    void EventsManager::setEvent(const std::string &event)
    {
        events.insert({event, std::map<Subscribable *, std::function<void(const std::optional<sf::Event> &)>>()});
    }

    void EventsManager::fire(const std::string &event, std::function<bool(Subscribable &)> filter,
                             std::function<void()> system, const std::optional<sf::Event> &e)
    {
        auto event_map = getEvent(event);
        if (!event_map)
        {
            return;
        }

        beginBusy();

        // Make a copy of the callbacks to avoid iterator invalidation
        auto callbacksCopy = *event_map.value();

        for (auto &[component, callback] : callbacksCopy)
        {
            auto &ep = *component;

            if (!filter || filter(ep))
            {
                // Check if component still exists (might have been unsubscribed)
                auto currentEventMap = getEvent(event);
                if (currentEventMap && currentEventMap.value()->count(component))
                {
                    callback(e);
                }
            }
        }

        if (system)
        {
            system();
        }

        endBusy();
    }

    void EventsManager::unsubscribe(const std::string &event, Subscribable *component)
    {
        deferOrExecute([=]() {
            doUnsubscribe(event, component);
        });
    }

    void EventsManager::unsubscribeAll(Subscribable *component)
    {
        deferOrExecute([=]() {
            doUnsubscribeAll(component);
        });
    }

    void EventsManager::subscribe(const std::string &event, Subscribable *component,
                                  std::function<void(const std::optional<sf::Event> &event)> f)
    {
        auto opt_map = getEvent(event);
        if (!opt_map)
        {
            setEvent(event);
            opt_map = getEvent(event);
        }
        auto &event_map = *opt_map.value();
        event_map[component] = f;
    }

    void EventsManager::clearAllEvents()
    {
        deferOrExecute([]() {
            events.clear();
            DeferredOperationsManager<EventsManager>::clearPending();
        });
    }

    void EventsManager::doUnsubscribe(const std::string &event, Subscribable *component)
    {
        auto it = events.find(event);
        if (it == events.end())
        {
            return;
        }

        it->second.erase(component);

        if (it->second.empty())
        {
            events.erase(it);
        }
    }

    void EventsManager::doUnsubscribeAll(Subscribable *component)
    {
        for (auto it = events.begin(); it != events.end();)
        {
            it->second.erase(component);

            if (it->second.empty())
            {
                it = events.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void EventsManager::forceUnsubscribeAll(Subscribable *component)
    {
        doUnsubscribeAll(component);
    }

} // namespace ml