//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef EVENTSMANAGER_H
#define EVENTSMANAGER_H

#include <any>
#include <map>
#include <string>

namespace ml {




class UIComponent;
class Controller;
class EventsManager
{
private:
    inline static std::map<std::string, std::map<void *, std::function<void()>>> events;
public:
    static std::optional<std::map<void *, std::function<void()>>*> getEvent(const std::string& event);
    static void setEvent(const std::string& event);
    static void fire(const std::string& event, std::function<bool(UIComponent&)> filter = [](UIComponent&){return true;}, std::function<void(UIComponent&)> system = nullptr);
    // static void fire(const std::string& event, const std::vector<UIComponent*>& components, std::function<bool(UIComponent&)> filter = [](UIComponent&){return true;}, std::function<void(UIComponent&)> system = nullptr);
    // static void fire(const std::string& event, const std::vector<Controller*>& components, std::function<bool(UIComponent&)> filter = [](UIComponent&){return true;}, std::function<void(UIComponent&)> system = nullptr);
    // static void clearAllEvents();
    // static void subscribe(void* component, const std::string& event, std::function<void()>);
    // static void unsubscribe(void * component, const std::string& event);
    // static void unsubscribeAll(void * component);
};
} // namespace ml
#endif //EVENTSMANAGER_H


