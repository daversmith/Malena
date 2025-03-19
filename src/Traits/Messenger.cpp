//
// Created by Dave R. Smith on 3/5/25.
//

#include "Messenger.h"
#include "../Managers/ComponentsManager.h"

namespace ml {
void Messenger::onClick(std::function<void()>f)
{
    subscribe("click", f);
}
void Messenger::onUpdate(std::function<void()>f)
{
    subscribe("update", f);
}
    void Messenger::onHover(std::function<void()>f)
{
    subscribe("hover", f);
}
void Messenger::subscribe(const std::string &event, std::function<void()> f)
{
    auto opt_map = EventsManager::getEvent(event);
    if ( !opt_map) {
        EventsManager::setEvent(event);
        opt_map = EventsManager::getEvent(event);
    }
    auto& event_map = *opt_map.value();
    event_map[this] = f;
}

void Messenger::publish(const std::string &event, ComponentsManager& app_controller, std::function<bool(UIComponent&)> filter)
{
    EventsManager::fire(event, filter);
}
} // namespace ml

