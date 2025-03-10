//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H


#include "EventsManager.h"
#include "StateManager.h"
class AppController;
class MessageManager //: public StateManager
{

public:
    void onClick(std::function<void()>);
    void onUpdate(std::function<void()>);
    void subscribe(const std::string& event, std::function<void()>);
    static void publish(const std::string& event, AppController& app_controller, std::function<bool(UIComponent&)> filter = [](UIComponent&){return true;});
};



#endif //MESSAGEMANAGER_H
