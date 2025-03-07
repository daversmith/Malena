//
// Created by Dave R. Smith on 3/5/25.
//
#include "EventsManager.h"
#include "Component.h"

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
std::map<void *, std::function<void()>> *EventsManager::getCurrentEvent()
{
    return currentEvent;
}
void EventsManager::setFilter(std::function<bool(Component&)> filter)
{
    EventsManager::filter = filter;
}
void EventsManager::reset()
{
    currentEvent = nullptr;
    filter = nullptr;
}
std::function<bool(Component&)> EventsManager::getFilter()
{
    return filter;
}
void EventsManager::setCurrentEvent(const std::string& event)
{
    currentEvent = getEvent(event).value();
}
void EventsManager::setCurrentEvent(const std::string& event, std::function<bool(Component&)> filter)
{
    setCurrentEvent(event);
    setFilter(filter);
}
void EventsManager::process(const std::vector<Component*>& components)
{
    auto event_map = EventsManager::getCurrentEvent();
    if(event_map)
    {
        for(auto &c : components)
        {
            if(EventsManager::getFilter()(*c))
            {
                if (auto func_it = event_map->find(c); func_it != event_map->end()) {
                    func_it->second();
                }

            }
        }
    }
    EventsManager::reset();
}

void EventsManager::fire(const std::string& event, std::function<bool(Component&)> filter)
{
    EventsManager::setCurrentEvent(event);
    EventsManager::setFilter(filter);
}