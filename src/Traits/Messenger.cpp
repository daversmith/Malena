//
// Created by Dave R. Smith on 3/5/25.
//

#include <functional>
#include <utility>

#include <melena/Traits/Messenger.h>
#include <melena/Managers/ComponentsManager.h>

namespace ml {
    void Messenger::onClick(std::function<void()>f)
    {
        subscribe("click", std::move(f));
    }
    void Messenger::onClick(std::function<void(const std::optional<sf::Event> &event)>f)
    {
        subscribe("click", std::move(f));
    }
    void Messenger::onUpdate(std::function<void()>f)
    {
        subscribe("update", std::move(f));
    }
    void Messenger::onUpdate(std::function<void(const std::optional<sf::Event> &event)>f)
    {
        subscribe("update", std::move(f));
    }
    void Messenger::onHover(std::function<void()>f)
    {
        subscribe("hover", std::move(f));
    }
    void Messenger::onHover(std::function<void(const std::optional<sf::Event> &event)>f)
    {
        subscribe("hover", std::move(f));
    }
    void Messenger::onUnhover(std::function<void()>f)
    {
        subscribe("unhover", f);
    }
    void Messenger::onUnhover(std::function<void(const std::optional<sf::Event> &event)>f)
    {
        subscribe("unhover", f);
    }

    void Messenger::onTextEntered(const std::function<void(const std::optional<sf::Event> &event)>& f)
    {
        subscribe("textentered", f);
    }

    void Messenger::onTextEntered(std::function<void()> f)
    {

        subscribe("textentered", std::move(f));
    }

    void Messenger::onFocus(std::function<void(const std::optional<sf::Event> &event)>f)
    {
        subscribe("focus", std::move(f));
    }

    void Messenger::onFocus(std::function<void()> f)
    {
        subscribe("focus", std::move(f));
    }

    void Messenger::onBlur(std::function<void(const std::optional<sf::Event> &event)>f)
    {
        subscribe("blur", std::move(f));
    }

    void Messenger::onBlur(std::function<void()> f)
    {
        subscribe("blur", std::move(f));
    }

    void Messenger::onKeypress(std::function<void(const std::optional<sf::Event> &event)>f)
    {
        subscribe("keypress", std::move(f));
    }

    void Messenger::onKeypress(std::function<void()> f)
    {
        subscribe("keypress", std::move(f));
    }

    void Messenger::onMouseMoved(std::function<void(const std::optional<sf::Event> &event)>f)
    {
        subscribe("mousemoved", std::move(f));
    }

    void Messenger::onMouseMoved(std::function<void()> f)
    {
        subscribe("mousemoved", std::move(f));
    }

    void Messenger::subscribe(const std::string &event, std::function<void(const std::optional<sf::Event> &event)> f)
    {
        EventsManager::subscribe(event, reinterpret_cast<UIComponent*>(this), f);
    }

    void Messenger::subscribe(const std::string &event, std::function<void()>f)
    {
        auto wrapped = [f](const std::optional<sf::Event>&)
        {
            f();
        };

        subscribe(event, wrapped);
    }

    void Messenger::unsubscribe(const std::string &event)
    {
        EventsManager::unsubscribe(event, reinterpret_cast<UIComponent*>(this));
    }

    void Messenger::unsubscribeAll()
    {
        EventsManager::unsubscribeAll(reinterpret_cast<UIComponent*>(this));
    }

    void Messenger::publish(const std::string &event, std::function<bool(UIComponent&)> filter)
    {
        EventsManager::fire(event, std::move(filter));
    }
} // namespace ml

