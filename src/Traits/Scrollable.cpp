//
// Scrollable.cpp
//

#include <Malena/Core/Core.h>
#include <Malena/Engine/Events/EventManager.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <Malena/Traits/Scrollable.h>
#include <Malena/Utilities/MouseEvents.h>

namespace ml
{
    // ── Scrollable ────────────────────────────────────────────────────────────

    void Scrollable::onScroll(std::function<void()> f)
    {
        EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
        Fireable::addCallback(Event::SCROLL, this, std::move(cb));
    }

    void Scrollable::onScroll(std::function<void(const std::optional<sf::Event>&)> f)
    {
        Fireable::addCallback(Event::SCROLL, this, std::move(f));
    }

    void Scrollable::onMouseMoved(std::function<void()> f)
    {
        EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
        Fireable::addCallback(Event::MOUSE_MOVED, this, std::move(cb));
    }

    void Scrollable::onMouseMoved(std::function<void(const std::optional<sf::Event>&)> f)
    {
        Fireable::addCallback(Event::MOUSE_MOVED, this, std::move(f));
    }

    void Scrollable::onMousePressed(std::function<void()> f)
    {
        EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
        Fireable::addCallback(Event::MOUSE_PRESSED, this, std::move(cb));
    }

    void Scrollable::onMousePressed(std::function<void(const std::optional<sf::Event>&)> f)
    {
        Fireable::addCallback(Event::MOUSE_PRESSED, this, std::move(f));
    }

    void Scrollable::onMouseReleased(std::function<void()> f)
    {
        EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
        Fireable::addCallback(Event::MOUSE_RELEASED, this, std::move(cb));
    }

    void Scrollable::onMouseReleased(std::function<void(const std::optional<sf::Event>&)> f)
    {
        Fireable::addCallback(Event::MOUSE_RELEASED, this, std::move(f));
    }

    // ── ScrollableDispatcher ──────────────────────────────────────────────────

    bool ScrollableDispatcher::occurred(const std::optional<sf::Event>& event)
    {
        return event.has_value() && (
            event->is<sf::Event::MouseWheelScrolled>() ||
            event->is<sf::Event::MouseMoved>()         ||
            event->is<sf::Event::MouseButtonPressed>() ||
            event->is<sf::Event::MouseButtonReleased>());
    }

    bool ScrollableDispatcher::filter(const std::optional<sf::Event> &event, Core *component)
    {
        // mousemoved fires for all components
        // scroll, pressed, released fire only when mouse is over component
        if (event.has_value() && event->is<sf::Event::MouseMoved>())
            return true;

        auto* positionable = dynamic_cast<Positionable*>(component);
        if (!positionable) return false;
        return MouseEvents::isHovered(*positionable, WindowManager::getWindow());
    }

    void ScrollableDispatcher::fire(const std::optional<sf::Event>& event)
    {
        if (!event.has_value()) return;

        if (event->is<sf::Event::MouseWheelScrolled>())
            EventManager::fire(Event::SCROLL, this, event);

        if (event->is<sf::Event::MouseMoved>())
            EventManager::fire(Event::MOUSE_MOVED, this, event);

        if (event->is<sf::Event::MouseButtonPressed>())
            EventManager::fire(Event::MOUSE_PRESSED, this, event);

        if (event->is<sf::Event::MouseButtonReleased>())
            EventManager::fire(Event::MOUSE_RELEASED, this, event);
    }

} // namespace ml