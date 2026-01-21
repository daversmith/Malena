//
// EventsManager.cpp
//

#include <functional>
#include <iostream>
#include <optional>
#include <Malena/Managers/EventsManager.h>

namespace ml
{
    std::optional<std::map<UIComponent *, std::function<void(const std::optional<sf::Event> &)>> *>
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
        events.insert({event, std::map<UIComponent *, std::function<void(const std::optional<sf::Event> &)>>()});
    }

    void EventsManager::fire(const std::string &event, std::function<bool(UIComponent &)> filter,
                             std::function<void()> system, const std::optional<sf::Event> &e)
    {
        auto event_map = getEvent(event);
        if (!event_map)
        {
            return;
        }

        beginBusy();  // ✅ Using base class method

        // Make a copy of the callbacks to avoid iterator invalidation
        auto callbacksCopy = *event_map.value();

        for (auto &[component, callback] : callbacksCopy)
        {
            auto &c = *static_cast<UIComponent *>(component);

            if (!filter || filter(c))
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

        endBusy();  // ✅ Using base class method - auto-processes pending
    }

    void EventsManager::unsubscribe(const std::string &event, UIComponent *component)
    {
        deferOrExecute([=]() {  // ✅ Using base class method
            doUnsubscribe(event, component);
        });
    }

    void EventsManager::unsubscribeAll(UIComponent *component)
    {
        deferOrExecute([=]() {  // ✅ Using base class method
            doUnsubscribeAll(component);
        });
    }

    void EventsManager::subscribe(const std::string &event, UIComponent *component,
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
            DeferredOperationsManager<EventsManager>::clearPending();  // Clear any pending operations too
        });
    }

    // Internal implementation methods

    void EventsManager::doUnsubscribe(const std::string &event, UIComponent *component)
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

    void EventsManager::doUnsubscribeAll(UIComponent *component)
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

    void EventsManager::forceUnsubscribeAll(UIComponent *component)
    {
        // Force immediate - bypass deferral (for plugin unloading)
        doUnsubscribeAll(component);
    }

} // namespace ml