//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef EVENTSMANAGER_H
#define EVENTSMANAGER_H

#include <any>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>
namespace ml {




class UIComponent;
class Controller;
class EventsManager
{
private:
    inline static std::map<std::string, std::map<UIComponent *, std::function<void(const std::optional<sf::Event> &event)>>> events;
public:
    static std::optional<std::map<UIComponent *, std::function<void(const std::optional<sf::Event> &event)>>*> getEvent(const std::string& event);
    static void setEvent(const std::string& event);
    static void fire(const std::string& event, std::function<bool(UIComponent&)> filter = [](UIComponent&){return true;}, std::function<void()> system = nullptr, const std::optional<sf::Event>&e = std::nullopt);
    // static void fire(const std::string& event, const std::vector<UIComponent*>& components, std::function<bool(UIComponent&)> filter = [](UIComponent&){return true;}, std::function<void(UIComponent&)> system = nullptr);
    // static void fire(const std::string& event, const std::vector<Controller*>& components, std::function<bool(UIComponent&)> filter = [](UIComponent&){return true;}, std::function<void(UIComponent&)> system = nullptr);
    // static void clearAllEvents();
    // static void subscribe(void* component, const std::string& event, std::function<void()>);
    // static void unsubscribe(void * component, const std::string& event);
    // static void unsubscribeAll(void * component);
};
} // namespace ml
#endif //EVENTSMANAGER_H


