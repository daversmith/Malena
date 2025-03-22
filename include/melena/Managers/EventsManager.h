//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef EVENTSMANAGER_H
#define EVENTSMANAGER_H

#include <any>
#include <map>
#include <string>
#include <functional>
#include <SFML/Graphics.hpp>
namespace ml {




class UIComponent;
class Controller;
class EventsManager
{
private:
    inline static std::map<std::string, std::map<UIComponent *, std::function<void(const std::optional<sf::Event> &event)>>> events;
    inline static bool clearPending = false;
    inline static int fireDepth = 0;
public:
    static std::optional<std::map<UIComponent *, std::function<void(const std::optional<sf::Event> &event)>>*> getEvent(const std::string& event);
    static void setEvent(const std::string& event);
    static void fire(const std::string& event, std::function<bool(UIComponent&)> filter = [](UIComponent&){return true;}, std::function<void()> system = nullptr, const std::optional<sf::Event>&e = std::nullopt);

    static void subscribe(const std::string& event,
                      UIComponent* component,
                      std::function<void(const std::optional<sf::Event>&)> callback);

    static void unsubscribe(const std::string& event,
                            UIComponent* component);

    static void unsubscribeAll(UIComponent* component);
    static void clearAllEvents();

};
} // namespace ml
#endif //EVENTSMANAGER_H


