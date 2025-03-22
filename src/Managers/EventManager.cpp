
#include <functional>
#include <optional>

#include <melena/Managers/EventsManager.h>

namespace ml {

    std::optional<std::map<UIComponent*, std::function<void(const std::optional<sf::Event>&)>>*> EventsManager::getEvent(const std::string& event) {
        auto it = events.find(event);
        if (it != events.end()) {
            return &it->second;
        }
        return std::nullopt;
    }
    void EventsManager::setEvent(const std::string& event)
    {
        events.insert({event, std::map<UIComponent*, std::function<void(const std::optional<sf::Event>&)>>()});
    }
    void EventsManager::fire(const std::string& event, std::function<bool(UIComponent&)> filter, std::function<void()> system, const std::optional<sf::Event>&e)
    {

            auto event_map = getEvent(event);
            if(event_map)
            {
                ++fireDepth;
                for(auto &[component, callback] : *event_map.value())
                {
                    auto &c = *static_cast<UIComponent*>(component);

                    if(!filter || filter(c)) // order matters
                    {
                        callback(e);
                    }
                }
                if(system)
                {
                    system();
                }
                --fireDepth;

                if(fireDepth == 0 && clearPending) {
                    events.clear();
                    clearPending = false;
                }
            }
    }
    void EventsManager::unsubscribe(const std::string& event, UIComponent* component)
    {
        auto it = events.find(event);
        if(it == events.end()) return;

        it->second.erase(component);
        if(it->second.empty()) {
            events.erase(it);
        }
    }

    void EventsManager::unsubscribeAll(UIComponent* component)
    {
        for(auto it = events.begin(); it != events.end(); )
        {
            it->second.erase(component);
            if(it->second.empty()) {
                it = events.erase(it);
            } else {
                ++it;
            }
        }
    }

    void EventsManager::subscribe(const std::string &event, UIComponent* component, std::function<void(const std::optional<sf::Event> &event)> f)
    {
        auto opt_map = getEvent(event);
        if ( !opt_map) {
            setEvent(event);
            opt_map = getEvent(event);
        }
        auto& event_map = *opt_map.value();
        event_map[component] = f;
    }
    void EventsManager::clearAllEvents()
    {
        if(fireDepth > 0) {
            clearPending = true;
        } else {
            events.clear();
        }
    }
} // namespace ml

