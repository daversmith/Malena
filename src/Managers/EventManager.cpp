//
// Created by Dave R. Smith on 3/5/25.
//
#include "EventsManager.h"
#include "../Interfaces/UIComponent.h"
#include "../Controllers/Controller.h"

namespace ml {

std::optional<std::map<void*, std::function<void()>>*> EventsManager::getEvent(const std::string& event) {
    auto it = events.find(event);
    if (it != events.end()) {
        return &it->second;
    }
    return std::nullopt;
}
void EventsManager::setEvent(const std::string& event)
{
    events.insert({event, std::map<void*, std::function<void()>>()});
}
void EventsManager::fire(const std::string& event, std::function<bool(UIComponent&)> filter, std::function<void(UIComponent&)> system)
{
    auto event_map = getEvent(event);
    if(event_map)
    {
        for(auto &[component, callback] : *event_map.value())
        {
            auto &c = *static_cast<UIComponent*>(component);
            if(filter(c))
            {
                callback();
                if(system)
                    system(c);
            }
        }
    }

}
} // namespace ml

