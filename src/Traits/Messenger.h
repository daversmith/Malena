//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H
#include "../Managers/EventsManager.h"
#include "../Traits/Stateful.h"

namespace ml {
class ComponentsManager;
class Messenger
{

public:
    void onClick(std::function<void()>);
    void onUpdate(std::function<void()>);
    void onHover(std::function<void()>);
    void subscribe(const std::string& event, std::function<void()>) ;
    static void publish(const std::string& event, ComponentsManager& app_controller, std::function<bool(UIComponent&)> filter = [](UIComponent&){return true;});
};


} // namespace ml
#endif //MESSAGEMANAGER_H


