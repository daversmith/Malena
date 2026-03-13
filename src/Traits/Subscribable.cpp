//
// Created by Dave R. Smith on 3/5/25.
//

#include <functional>
#include <utility>
#include <iostream>

#include <Malena/Managers/ComponentsManager.h>
#include <Malena/Traits/Subscribable.h>

namespace ml
{
    Subscribable::Subscribable()
    {
    }

    Subscribable::~Subscribable()
    {
        unsubscribeAll();
    }

    void Subscribable::onClick(std::function<void()> f)
    {
        subscribe("click", std::move(f));
    }

    void Subscribable::onClick(std::function<void(const std::optional<sf::Event> &event)> f)
    {
        subscribe("click", std::move(f));
    }

    void Subscribable::onUpdate(std::function<void()> f)
    {
        subscribe("update", std::move(f));
    }

    void Subscribable::onUpdate(std::function<void(const std::optional<sf::Event> &event)> f)
    {
        subscribe("update", std::move(f));
    }

    void Subscribable::onHover(std::function<void()> f)
    {
        subscribe("hover", std::move(f));
    }

    void Subscribable::onHover(std::function<void(const std::optional<sf::Event> &event)> f)
    {
        subscribe("hover", std::move(f));
    }

    void Subscribable::onUnhover(std::function<void()> f)
    {
        subscribe("unhover", std::move(f));
    }

    void Subscribable::onUnhover(std::function<void(const std::optional<sf::Event> &event)> f)
    {
        subscribe("unhover", std::move(f));
    }

    void Subscribable::onTextEntered(const std::function<void(const std::optional<sf::Event> &event)> &f)
    {
        subscribe("textentered", f);
    }

    void Subscribable::onTextEntered(std::function<void()> f)
    {
        subscribe("textentered", std::move(f));
    }

    void Subscribable::onFocus(std::function<void(const std::optional<sf::Event> &event)> f)
    {
        subscribe("focus", std::move(f));
    }

    void Subscribable::onFocus(std::function<void()> f)
    {
        subscribe("focus", std::move(f));
    }

    void Subscribable::onBlur(std::function<void(const std::optional<sf::Event> &event)> f)
    {
        subscribe("blur", std::move(f));
    }

    void Subscribable::onBlur(std::function<void()> f)
    {
        subscribe("blur", std::move(f));
    }

    void Subscribable::onKeypress(std::function<void(const std::optional<sf::Event> &event)> f)
    {
        subscribe("keypress", std::move(f));
    }

    void Subscribable::onKeypress(std::function<void()> f)
    {
        subscribe("keypress", std::move(f));
    }

    void Subscribable::onMouseMoved(std::function<void(const std::optional<sf::Event> &event)> f)
    {
        subscribe("mousemoved", std::move(f));
    }

    void Subscribable::onMouseMoved(std::function<void()> f)
    {
        subscribe("mousemoved", std::move(f));
    }

    void Subscribable::onMouseWheel(std::function<void()> f)
    {
        subscribe("mousewheel", std::move(f));
    }

    void Subscribable::subscribe(const std::string &event, std::function<void(const std::optional<sf::Event> &event)> f)
    {
        EventsManager::subscribe(event, this, std::move(f));
    }

    void Subscribable::subscribe(const std::string &event, std::function<void()> f)
    {
        auto wrapped = [f = std::move(f)](const std::optional<sf::Event> &) {
            if (f) f();
            else std::cerr << "ERROR: Callback function is null during execution!" << std::endl;
        };
        subscribe(event, std::move(wrapped));
    }

    void Subscribable::unsubscribe(const std::string &event)
    {
        EventsManager::unsubscribe(event, this);
    }

    void Subscribable::unsubscribeAll()
    {
        EventsManager::unsubscribeAll(this);
    }

    void Subscribable::publish(const std::string &event, std::function<bool(Subscribable &)> filter)
    {
        EventsManager::fire(event, std::move(filter));
    }

} // namespace ml