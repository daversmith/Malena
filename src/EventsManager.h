//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef EVENTSMANAGER_H
#define EVENTSMANAGER_H
#include <any>
#include <map>
#include <string>


class UIComponent;
class EventsManager
{
private:
    inline static std::map<std::string, std::map<void *, std::function<void()>>> events;
    inline static std::map<void *, std::function<void()>> *currentEvent = nullptr;
    inline static std::function<bool(UIComponent&)> filter;
public:
    static std::optional<std::map<void *, std::function<void()>>*> getEvent(const std::string& event);
    static void setEvent(const std::string& event);
    static void setCurrentEvent(const std::string& event);
    static void setCurrentEvent(const std::string& event, std::function<bool(UIComponent&)> filter);
    static std::map<void *, std::function<void()>> *getCurrentEvent();
    static void setFilter(std::function<bool(UIComponent&)> filter);
    static std::function<bool(UIComponent&)> getFilter();
    static void reset();
    static void process(const std::vector<UIComponent*>& components);
    static void fire(const std::string& event, std::function<bool(UIComponent&)> filter = [](UIComponent&){return true;});
};
#endif //EVENTSMANAGER_H
