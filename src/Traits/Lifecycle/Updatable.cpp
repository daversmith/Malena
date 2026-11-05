//
// Updatable.cpp
//

#include <Malena/Traits/Lifecycle/Updatable.h>
#include <Malena/Engine/Events/EventManager.h>
#include <Malena/Core/Core.h>
#include <iostream>

namespace ml
{
    // ── Updatable ─────────────────────────────────────────────────────────────

    void Updatable::onUpdate(std::function<void()> f, bool overwrite)
    {
        EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
        Fireable::addCallback(Event::UPDATE, this, std::move(cb), overwrite);
    }

    void Updatable::onUpdate(std::function<void(const std::optional<sf::Event>&)> f, bool overwrite)
    {
        Fireable::addCallback(Event::UPDATE, this, std::move(f), overwrite);
    }

    void Updatable::onWindowResized(std::function<void()> f, bool overwrite)
    {
        EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
        Fireable::addCallback(Event::WINDOW_RESIZED, this, std::move(cb), overwrite);
    }

    void Updatable::onWindowResized(std::function<void(const std::optional<sf::Event>&)> f, bool overwrite)
    {
        Fireable::addCallback(Event::WINDOW_RESIZED, this, std::move(f), overwrite);
    }

    void Updatable::onWindowFocusGained(std::function<void()> f, bool overwrite)
    {
        EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
        Fireable::addCallback(Event::WINDOW_FOCUS_GAINED, this, std::move(cb), overwrite);
    }

    void Updatable::onWindowFocusGained(std::function<void(const std::optional<sf::Event>&)> f, bool overwrite)
    {
        Fireable::addCallback(Event::WINDOW_FOCUS_GAINED, this, std::move(f), overwrite);
    }

    void Updatable::onWindowFocusLost(std::function<void()> f, bool overwrite)
    {
        EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
        Fireable::addCallback(Event::WINDOW_FOCUS_LOST, this, std::move(cb), overwrite);
    }

    void Updatable::onWindowFocusLost(std::function<void(const std::optional<sf::Event>&)> f, bool overwrite)
    {
        Fireable::addCallback(Event::WINDOW_FOCUS_LOST, this, std::move(f), overwrite);
    }

    // ── UpdatableDispatcher ───────────────────────────────────────────────────
    bool UpdatableDispatcher::filter(const std::optional<sf::Event>&, Core* component)
    {
        // Effective enabled state, so a disabled ancestor also stops updates.
        // Subscribers that aren't a Core (component == nullptr) always update.
        return component == nullptr || component->isEnabled();
    }

    void UpdatableDispatcher::fire()
    {
        // Always fire update every frame
        EventManager::fire(Event::UPDATE, this, std::optional<sf::Event>());

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