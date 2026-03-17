//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#pragma once

#include <Malena/Engine/Events/EventsManager.h>
#include <Malena/Resources/FlagManager.h>
#include <string>
#include <Malena/Traits/Base/Trait.h>

namespace ml
{
    class Core;

    /**
     * @brief Trait that exposes the full @c EventsManager subscription API.
     *
     * @c Subscribable wraps @c EventsManager with named convenience methods
     * for every framework-level event. It is one of the three traits
     * automatically included in every @c ml::Core object and is the primary
     * way user code reacts to input, updates, and lifecycle events.
     *
     * ### How it works
     * Each named method (e.g., @c onClick, @c onHover) registers a callback
     * against a string event key in @c EventsManager. @c UIManager fires those
     * keys each frame based on SFML input. The deferred-removal mechanism in
     * @c EventsManager ensures that unsubscribing inside a callback is safe.
     *
     * The destructor calls @c unsubscribeAll(), so callbacks are always
     * cleaned up when the owning object is destroyed — no manual teardown needed.
     *
     * ### Callback overloads
     * Every method has two overloads:
     * - `void()` — use when you do not need the raw SFML event data
     * - `void(const std::optional<sf::Event>&)` — use when you need key codes,
     *   mouse position, scroll delta, etc.
     *
     * ### Usage
     * @code
     * // Simple click handler
     * myRect.onClick([] { std::cout << "clicked\n"; });
     *
     * // Hover with SFML event data
     * myRect.onHover([](const std::optional<sf::Event>& e) {
     *     // e contains the raw MouseMoved event
     * });
     *
     * // Per-frame update
     * myRect.onUpdate([this] { _angle += 1.f; });
     *
     * // Raw subscription to a custom event name
     * myRect.subscribe("my_custom_event", [] { /* ... *\/ });
     * @endcode
     *
     * @see EventsManager, Core, Messenger
     */
    class Subscribable : public Trait
    {
    public:
        /// Convenience typedef for the event parameter type used in callbacks.
        typedef const std::optional<sf::Event>& Event;

        virtual ~Subscribable();
        Subscribable();

        // ── Input events ─────────────────────────────────────────────────────

        /**
         * @brief Register a callback fired when the mouse button is released
         *        over this component.
         * @param callback Invoked with no arguments.
         */
        void onClick(std::function<void()>);

        /**
         * @brief Register a callback fired when the mouse button is released
         *        over this component.
         * @param callback Invoked with the raw SFML event.
         */
        void onClick(std::function<void(const std::optional<sf::Event>& event)>);

        /**
         * @brief Register a callback fired when the mouse enters this
         *        component's bounds.
         * @param callback Invoked with no arguments.
         */
        void onHover(std::function<void()>);

        /**
         * @brief Register a callback fired when the mouse enters this
         *        component's bounds.
         * @param callback Invoked with the raw SFML event.
         */
        void onHover(std::function<void(const std::optional<sf::Event>& event)>);

        /**
         * @brief Register a callback fired when the mouse leaves this
         *        component's bounds.
         * @param callback Invoked with no arguments.
         */
        void onUnhover(std::function<void()>);

        /**
         * @brief Register a callback fired when the mouse leaves this
         *        component's bounds.
         * @param callback Invoked with the raw SFML event.
         */
        void onUnhover(std::function<void(const std::optional<sf::Event>& event)>);

        /**
         * @brief Register a callback fired when a key is pressed while
         *        this component has focus.
         * @param callback Invoked with no arguments.
         */
        void onKeypress(std::function<void()> f);

        /**
         * @brief Register a callback fired when a key is pressed while
         *        this component has focus.
         * @param callback Invoked with the raw SFML event (contains key code).
         */
        void onKeypress(std::function<void(const std::optional<sf::Event>& event)>);

        /**
         * @brief Register a callback fired when a Unicode character is
         *        entered (i.e., @c sf::Event::TextEntered).
         * @param callback Invoked with no arguments.
         */
        void onTextEntered(std::function<void()> f);

        /**
         * @brief Register a callback fired when a Unicode character is
         *        entered.
         * @param callback Invoked with the raw SFML event (contains Unicode value).
         */
        void onTextEntered(const std::function<void(const std::optional<sf::Event>& event)>&);

        /**
         * @brief Register a callback fired when the mouse moves anywhere
         *        in the window.
         * @param callback Invoked with no arguments.
         */
        void onMouseMoved(std::function<void()> f);

        /**
         * @brief Register a callback fired when the mouse moves anywhere
         *        in the window.
         * @param callback Invoked with the raw SFML event (contains new position).
         */
        void onMouseMoved(std::function<void(const std::optional<sf::Event>& event)>);

        /**
         * @brief Register a callback fired when the mouse wheel is scrolled.
         * @param callback Invoked with no arguments.
         */
        void onMouseWheel(std::function<void()> f);

        // ── Focus / blur ─────────────────────────────────────────────────────

        /**
         * @brief Register a callback fired when this component gains
         *        keyboard focus.
         * @param callback Invoked with no arguments.
         */
        void onFocus(std::function<void()> f);

        /**
         * @brief Register a callback fired when this component gains
         *        keyboard focus.
         * @param callback Invoked with the raw SFML event.
         */
        void onFocus(std::function<void(const std::optional<sf::Event>& event)>);

        /**
         * @brief Register a callback fired when this component loses
         *        keyboard focus.
         * @param callback Invoked with no arguments.
         */
        void onBlur(std::function<void()> f);

        /**
         * @brief Register a callback fired when this component loses
         *        keyboard focus.
         * @param callback Invoked with the raw SFML event.
         */
        void onBlur(std::function<void(const std::optional<sf::Event>& event)>);

        // ── Per-frame update ─────────────────────────────────────────────────

        /**
         * @brief Register a callback fired every frame during the update tick.
         * @param callback Invoked with no arguments.
         */
        void onUpdate(std::function<void()>);

        /**
         * @brief Register a callback fired every frame during the update tick.
         * @param callback Invoked with the raw SFML event (may be empty optional
         *                 on update-only ticks with no pending SFML event).
         */
        void onUpdate(std::function<void(const std::optional<sf::Event>& event)>);

        // ── Raw subscription ─────────────────────────────────────────────────

        /**
         * @brief Subscribe to an arbitrary named event.
         *
         * Use this to react to custom events fired via @c EventsManager::fire()
         * or @c publish(). The event name can be any string; the framework
         * fires its built-in events by convention (e.g., @c "click",
         * @c "draggable", @c "update").
         *
         * @param event    The event name to subscribe to.
         * @param callback Invoked with the raw SFML event when fired.
         */
        void subscribe(const std::string& event,
                       std::function<void(const std::optional<sf::Event>& event)>);

        /**
         * @brief Subscribe to an arbitrary named event (no-argument overload).
         *
         * @param event    The event name to subscribe to.
         * @param callback Invoked with no arguments when fired.
         */
        void subscribe(const std::string& event, std::function<void()>);

        /**
         * @brief Unsubscribe from a single named event.
         *
         * Safe to call from within a callback for that same event —
         * the removal is deferred until the current @c EventsManager
         * iteration completes.
         *
         * @param event The event name to unsubscribe from.
         */
        void unsubscribe(const std::string& event);

        /**
         * @brief Unsubscribe from all events this object is registered for.
         *
         * Called automatically by the @c Core destructor chain, so explicit
         * calls are only needed in unusual teardown scenarios.
         */
        void unsubscribeAll();

        // ── Static publish ───────────────────────────────────────────────────

        /**
         * @brief Fire a named event to all matching subscribers.
         *
         * Delegates directly to @c EventsManager::fire(). The optional
         * @p filter predicate allows the caller to restrict delivery to a
         * subset of subscribers (e.g., only the currently focused component).
         *
         * @param event  The event name to fire.
         * @param filter Predicate called for each registered @c Subscribable;
         *               the callback is invoked only when the predicate returns
         *               @c true. Defaults to delivering to all subscribers.
         */
        static void publish(
            const std::string& event,
            std::function<bool(Subscribable&)> filter = [](Subscribable&) { return true; });
    };

} // namespace ml

#endif // MESSAGEMANAGER_H
