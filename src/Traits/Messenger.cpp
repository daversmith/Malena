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
    void Messenger::onClick(std::function<void(const std::optional<sf::Event> &event)>f)
    {
        subscribe("click", f);
    }
    void Messenger::onUpdate(std::function<void()>f)
    {
        subscribe("update", f);
    }
    void Messenger::onUpdate(std::function<void(const std::optional<sf::Event> &event)>f)
    {
        subscribe("update", f);
    }
    void Messenger::onHover(std::function<void()>f)
    {
        subscribe("hover", f);
    }
    void Messenger::onHover(std::function<void(const std::optional<sf::Event> &event)>f)
    {
        subscribe("hover", f);
    }
    void Messenger::onUnhover(std::function<void()>f)
    {
        subscribe("unhover", f);
    }
    void Messenger::onUnhover(std::function<void(const std::optional<sf::Event> &event)>f)
    {
        subscribe("unhover", f);
    }

    void Messenger::onTextEntered(std::function<void(const std::optional<sf::Event> &event)>f)
    {
        subscribe("textentered", f);
    }

    void Messenger::onTextEntered(std::function<void()> f)
    {

        subscribe("textentered", f);
    }

    void Messenger::onFocus(std::function<void(const std::optional<sf::Event> &event)>f)
    {
        subscribe("focus", f);
    }

    void Messenger::onFocus(std::function<void()> f)
    {
        subscribe("focus", f);
    }

    void Messenger::onBlur(std::function<void(const std::optional<sf::Event> &event)>f)
    {
        subscribe("blur", f);
    }

    void Messenger::onBlur(std::function<void()> f)
    {
        subscribe("blur", f);
    }

    void Messenger::onKeypress(std::function<void(const std::optional<sf::Event> &event)>f)
    {
        subscribe("keypress", f);
    }

    void Messenger::onKeypress(std::function<void()> f)
    {
        subscribe("keypress", f);
    }

    void Messenger::onMouseMoved(std::function<void(const std::optional<sf::Event> &event)>f)
    {
        subscribe("mousemoved", f);
    }

    void Messenger::onMouseMoved(std::function<void()> f)
    {
        subscribe("mousemoved", f);
    }

    void Messenger::subscribe(const std::string &event, std::function<void(const std::optional<sf::Event> &event)> f)
    {
        auto opt_map = EventsManager::getEvent(event);
        if ( !opt_map) {
            EventsManager::setEvent(event);
            opt_map = EventsManager::getEvent(event);
        }
        auto& event_map = *opt_map.value();
        event_map[reinterpret_cast<UIComponent*>(this)] = f;
    }

    void Messenger::subscribe(const std::string &event, std::function<void()>f)
    {
        auto wrapped = [f](const std::optional<sf::Event>&)
        {
            f();
        };

        subscribe(event, wrapped);
    }

    void Messenger::publish(const std::string &event, std::function<bool(UIComponent&)> filter)
    {
        EventsManager::fire(event, filter);
    }
} // namespace ml

