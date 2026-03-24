//
// Updatable.cpp
//

#include <Malena/Engine/Events/EventManager.h>
#include <Malena/Traits/Updatable.h>
#include <iostream>

namespace ml
{
    // ── Updatable ─────────────────────────────────────────────────────────────

    void Updatable::onUpdate(std::function<void()> f)
    {
        EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
        Fireable::addCallback(Event::UPDATE, this, std::move(cb));
    }

    void Updatable::onUpdate(std::function<void(const std::optional<sf::Event>&)> f)
    {
        Fireable::addCallback(Event::UPDATE, this, std::move(f));
    }

    void Updatable::onWindowResized(std::function<void()> f)
    {
        EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
        Fireable::addCallback(Event::WINDOW_RESIZED, this, std::move(cb));
    }

    void Updatable::onWindowResized(std::function<void(const std::optional<sf::Event>&)> f)
    {
        Fireable::addCallback(Event::WINDOW_RESIZED, this, std::move(f));
    }

    void Updatable::onWindowFocusGained(std::function<void()> f)
    {
        EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
        Fireable::addCallback(Event::WINDOW_FOCUS_GAINED, this, std::move(cb));
    }

    void Updatable::onWindowFocusGained(std::function<void(const std::optional<sf::Event>&)> f)
    {
        Fireable::addCallback(Event::WINDOW_FOCUS_GAINED, this, std::move(f));
    }

    void Updatable::onWindowFocusLost(std::function<void()> f)
    {
        EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
        Fireable::addCallback(Event::WINDOW_FOCUS_LOST, this, std::move(cb));
    }

    void Updatable::onWindowFocusLost(std::function<void(const std::optional<sf::Event>&)> f)
    {
        Fireable::addCallback(Event::WINDOW_FOCUS_LOST, this, std::move(f));
    }

    // ── UpdatableDispatcher ───────────────────────────────────────────────────
    void UpdatableDispatcher::fire()
    {
        // Always fire update every frame
        EventsManager::fire(Event::UPDATE, this, std::optional<sf::Event>());

    }

    // void UpdatableDispatcher::fire(const std::optional<sf::Event>& event)
    // {
    //
    //     // Window events — only when the event applies
    //     if (!event.has_value()) return;
    //
    //     if (event->is<sf::Event::Resized>())
    //         _EventsManager::fire(Event::WINDOW_RESIZED, this, event);
    //
    //     if (event->is<sf::Event::FocusGained>())
    //         _EventsManager::fire(Event::WINDOW_FOCUS_GAINED, this, event);
    //
    //     if (event->is<sf::Event::FocusLost>())
    //         _EventsManager::fire(Event::WINDOW_FOCUS_LOST, this, event);
    // }

} // namespace ml