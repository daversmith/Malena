
#include "EventsManager.h"
#include "../Interfaces/UIComponent.h"
#include "../Controllers/Controller.h"

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
        }


    }
} // namespace ml

