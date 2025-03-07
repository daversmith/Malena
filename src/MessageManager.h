//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include "EventsManager.h"
class Component;
class MessageManager {

public:
    void onClick(std::function<void()>);
    void subscribe(const std::string& event, std::function<void()>);
    static void publish(const std::string& event, std::function<bool(Component&)> filter = [](Component&){return true;});
};



#endif //MESSAGEMANAGER_H
