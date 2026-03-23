//
// Keyable.cpp
//

#include <Malena/Traits/Keyable.h>
#include <Malena/Core/Core.h>
#include <Malena/Engine/Events/_EventsManager.h>
#include <Malena/Utilities/Flag.h>

namespace ml
{
    // ── Keyable ───────────────────────────────────────────────────────────────

    void Keyable::onKeypress(std::function<void()> f)
    {
        EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
        Fireable::addCallback(Event::KEYPRESS, this, std::move(cb));
    }

    void Keyable::onKeypress(std::function<void(const std::optional<sf::Event>&)> f)
    {
        Fireable::addCallback(Event::KEYPRESS, this, std::move(f));
    }

    void Keyable::onKeyRelease(std::function<void()> f)
    {
        EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
        Fireable::addCallback(Event::KEY_RELEASE, this, std::move(cb));
    }

    void Keyable::onKeyRelease(std::function<void(const std::optional<sf::Event>&)> f)
    {
        Fireable::addCallback(Event::KEY_RELEASE, this, std::move(f));
    }

    void Keyable::onTextEntered(std::function<void()> f)
    {
        EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
        Fireable::addCallback(Event::TEXT_ENTERED, this, std::move(cb));
    }

    void Keyable::onTextEntered(std::function<void(const std::optional<sf::Event>&)> f)
    {
        Fireable::addCallback(Event::TEXT_ENTERED, this, std::move(f));
    }

    // ── KeyableDispatcher ─────────────────────────────────────────────────────

    bool KeyableDispatcher::occurred(const std::optional<sf::Event>& event)
    {
        return event.has_value() && (
            event->is<sf::Event::KeyPressed>()   ||
            event->is<sf::Event::KeyReleased>()  ||
            event->is<sf::Event::TextEntered>());
    }

    bool KeyableDispatcher::filter(const std::optional<sf::Event> &event, Core *component)
    {
        // Only deliver to the focused component
        auto* core = dynamic_cast<Core*>(component);
        if (!core) return false;
        return true;//core->checkFlag(Flag::FOCUSED);
    }

    void KeyableDispatcher::fire(const std::optional<sf::Event>& event)
    {
        if (!event.has_value()) return;

        if (event->is<sf::Event::KeyPressed>())
            _EventsManager::fire(Event::KEYPRESS, this, event);

        if (event->is<sf::Event::KeyReleased>())
            _EventsManager::fire(Event::KEY_RELEASE, this, event);

        if (event->is<sf::Event::TextEntered>())
            _EventsManager::fire(Event::TEXT_ENTERED, this, event);
    }

} // namespace ml