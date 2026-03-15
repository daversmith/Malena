//
// EventsManager.h
//

#ifndef EVENTSMANAGER_H
#define EVENTSMANAGER_H

#pragma once

#include <SFML/Graphics.hpp>
#include <any>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <Malena/Core/DeferredOperationsManager.h>

namespace ml
{
    class Subscribable;
    class Controller;

    /**
     * @brief Centralized string-keyed publish-subscribe event bus.
     *
     * @c EventsManager is the backbone of Malena's input-driven event system.
     * Components subscribe to named events; @c UIManager fires those events
     * each frame based on SFML input. The two are fully decoupled — neither
     * needs a direct reference to the other.
     *
     * ### Event keys
     * Built-in event names fired by @c UIManager:
     *
     * | Key | When fired |
     * |-----|-----------|
     * | @c "click"     | Mouse button released over the component |
     * | @c "hover"     | Mouse entered the component's bounds |
     * | @c "unhover"   | Mouse left the component's bounds |
     * | @c "focus"     | Component gained keyboard focus |
     * | @c "blur"      | Component lost keyboard focus |
     * | @c "update"    | Every frame, during the update tick |
     * | @c "draggable" | Every frame, for drag handling in @c ComponentCore |
     * | @c "keypress"  | Key pressed while component has focus |
     * | @c "textEntered" | Unicode character entered |
     * | @c "mouseMoved"  | Mouse moved anywhere in the window |
     * | @c "mouseWheel"  | Mouse wheel scrolled |
     *
     * Custom event keys can also be used freely with @c subscribe and @c fire.
     *
     * ### Thread safety and iteration safety
     * @c EventsManager inherits @c DeferredOperationsManager<EventsManager>.
     * Any @c unsubscribe or @c unsubscribeAll call made while @c fire() is
     * iterating is automatically queued and processed after the iteration
     * completes. This makes it safe to unsubscribe from within a callback
     * without risking iterator invalidation.
     *
     * @note User code interacts with this class indirectly through the
     *       @c Subscribable trait methods (@c onClick, @c onHover, etc.).
     *       Direct calls to @c EventsManager are only needed for custom
     *       event names or framework-internal use.
     *
     * @see Subscribable, DeferredOperationsManager, UIController
     */
    class EventsManager : public DeferredOperationsManager<EventsManager>
    {
    private:
        /// Two-level map: event name → subscriber pointer → callback.
        inline static std::map<
            std::string,
            std::map<ml::Subscribable*, std::function<void(const std::optional<sf::Event>&)>>
        > events;

        inline static bool clearPending = false;

    public:
        /**
         * @brief Return the subscriber map for a named event, if it exists.
         *
         * Returns an optional pointer to the inner subscriber map for
         * @p event. Returns @c std::nullopt if no subscriber has registered
         * for that event name yet.
         *
         * @param event The event name to look up.
         * @return Optional pointer to the subscriber map, or @c std::nullopt.
         */
        static std::optional<
            std::map<Subscribable*, std::function<void(const std::optional<sf::Event>&)>>*
        > getEvent(const std::string& event);

        /**
         * @brief Ensure an event key exists in the event map.
         *
         * Creates an empty subscriber map for @p event if one does not
         * already exist. Rarely needed by user code; called internally
         * by @c subscribe.
         *
         * @param event The event name to register.
         */
        static void setEvent(const std::string& event);

        // ── Main API ─────────────────────────────────────────────────────────

        /**
         * @brief Fire a named event to all matching subscribers.
         *
         * Iterates the subscriber map for @p event and invokes each callback
         * for which @p filter returns @c true. After all component callbacks
         * have run, @p system is called if provided. The optional @p e is
         * forwarded to every callback.
         *
         * The manager marks itself busy for the duration of the iteration.
         * Any @c unsubscribe calls that arrive during this window are deferred
         * and processed automatically when @c fire returns.
         *
         * @param event   The event name to fire.
         * @param filter  Predicate called for each @c Subscribable; the callback
         *                is invoked only when the predicate returns @c true.
         *                Defaults to delivering to all subscribers.
         * @param system  Optional framework callback invoked after all component
         *                callbacks. Used by @c UIManager for post-event bookkeeping
         *                (e.g., updating focus state). Pass @c nullptr to skip.
         * @param e       SFML event forwarded to all callbacks. Pass
         *                @c std::nullopt for update-tick fires with no SFML event.
         */
        static void fire(
            const std::string& event,
            std::function<bool(Subscribable&)> filter = [](Subscribable&) { return true; },
            std::function<void()> system = nullptr,
            const std::optional<sf::Event>& e = std::nullopt);

        /**
         * @brief Register a callback for a named event on a specific subscriber.
         *
         * If @p component is already subscribed to @p event, the existing
         * callback is replaced. Safe to call at any time, including from
         * within a @c fire() iteration.
         *
         * @param event     The event name to subscribe to.
         * @param component The @c Subscribable that owns this subscription.
         * @param callback  Function invoked when the event fires for this component.
         */
        static void subscribe(
            const std::string& event,
            Subscribable* component,
            std::function<void(const std::optional<sf::Event>&)> callback);

        /**
         * @brief Remove a specific component's subscription to one event.
         *
         * If called during a @c fire() iteration, the removal is deferred
         * until the iteration completes.
         *
         * @param event     The event name to unsubscribe from.
         * @param component The subscriber to remove.
         */
        static void unsubscribe(const std::string& event, Subscribable* component);

        /**
         * @brief Remove all subscriptions belonging to @p component.
         *
         * Called automatically by @c Subscribable's destructor. If called
         * during a @c fire() iteration the removal is deferred, preventing
         * use-after-free crashes when a component destroys itself inside
         * a callback.
         *
         * @param component The subscriber to remove from all events.
         */
        static void unsubscribeAll(Subscribable* component);

        /**
         * @brief Remove all subscriptions for all events and all subscribers.
         *
         * Use during application teardown or full scene reset. If a
         * @c fire() iteration is in progress, the clear is deferred.
         */
        static void clearAllEvents();

        /**
         * @brief Unconditionally remove all subscriptions for @p component.
         *
         * Unlike @c unsubscribeAll, this bypasses the deferred-operation
         * mechanism and executes immediately regardless of busy state.
         * Only safe to call from @c PluginManager during plugin unloading,
         * where the plugin's memory is about to be freed and deferred
         * cleanup would leave a dangling pointer in the queue.
         *
         * @warning Do not call from user code or inside a @c fire() callback.
         *
         * @param component The subscriber to remove immediately.
         */
        static void forceUnsubscribeAll(Subscribable* component);

    private:
        /// Execute the actual single-event removal (called directly or from pending queue).
        static void doUnsubscribe(const std::string& event, Subscribable* component);

        /// Execute the actual full removal (called directly or from pending queue).
        static void doUnsubscribeAll(Subscribable* component);
    };

} // namespace ml

#endif // EVENTSMANAGER_H
