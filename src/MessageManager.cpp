//
// Created by Dave R. Smith on 3/5/25.
//

#include "MessageManager.h"
#include "Component.h"
void MessageManager::onClick(std::function<void()>f)
{
    subscribe("click", f);
}

void MessageManager::subscribe(const std::string &event, std::function<void()> f)
{
    auto opt_map = EventsManager::getEvent(event);
    if ( !opt_map) {
        EventsManager::setEvent(event);
        opt_map = EventsManager::getEvent(event);
    }
    auto& event_map = *opt_map.value();
    event_map[this] = f;
}

void MessageManager::publish(const std::string &event, std::function<bool(Component&)> filter)
{
    EventsManager::fire(event, filter);
}
