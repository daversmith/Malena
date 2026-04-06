// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 3/21/26.
//

#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H
#include "Fireable.h"
#include <optional>
namespace ml {

/**
 * @brief Base class for all per-event dispatchers in the Malena event system.
 * @ingroup EngineEvents
 *
 * @c EventDispatcher is the class you inherit from to create a new event trait.
 * It narrows @c Fireable to the SFML-event–driven path only: the no-argument
 * @c fire() and @c occurred() overloads inherited from @c Fireable are sealed
 * @c final so you only need to implement the two event-taking versions.
 *
 * ### How it fits together
 *
 * A complete event trait requires three pieces:
 *
 * 1. **A receiver class** — inherits @c EventReceiver and exposes a friendly
 *    callback registration method (@c onClick, @c onHover, etc.) that stores a
 *    callback via @c Fireable::addCallback.
 *
 * 2. **A dispatcher class** — inherits @c EventDispatcher and implements
 *    @c occurred(), @c filter(), and @c fire(). Registered as a singleton with
 *    the event system via @c ML_EXPORT.
 *
 * 3. **@c ML_EXPORT(MyDispatcher)** — placed after the class definition in the
 *    header. Creates the singleton and registers it with @c Fireable at startup.
 *
 * ### What each method does
 *
 * | Method | Called by | Purpose |
 * |--------|-----------|---------|
 * | @c occurred(event) | @c AppManager each frame | Return @c true when this SFML event (or frame tick) should trigger processing. |
 * | @c filter(event, component) | Framework, after @c occurred | Return @c true for each @c Core component that should receive this event. Default returns @c true for all. |
 * | @c fire(event) | Framework, after filtering | Iterate registered components and call @c process() on those that pass the filter. |
 *
 * ### Minimal example
 * @code
 * // ── MyTrait.h ─────────────────────────────────────────────────────────────
 *
 * // 1. Receiver — stores the callback and exposes the friendly API
 * class MyTrait : public ml::EventReceiver
 * {
 * public:
 *     void onMyEvent(std::function<void()> f)
 *     {
 *         Fireable::addCallback(ml::Event::CLICK, this,
 *             [f](const std::optional<sf::Event>&){ f(); });
 *     }
 * };
 *
 * // 2. Dispatcher — decides when and to whom to deliver
 * class MyTraitDispatcher : public ml::EventDispatcher
 * {
 * public:
 *     bool occurred(const std::optional<sf::Event>& event) override
 *     {
 *         // Return true when the triggering SFML event arrives
 *         return event && event->is<sf::Event::MouseButtonReleased>();
 *     }
 *
 *     bool filter(const std::optional<sf::Event>& event, ml::Core* component) override
 *     {
 *         // Optional: return false to skip components that shouldn't receive this
 *         return !component->checkFlag(ml::Flag::HIDDEN);
 *     }
 *
 *     void fire(const std::optional<sf::Event>& event) override
 *     {
 *         ml::EventManager::fire(ml::Event::CLICK, this, event);
 *     }
 * };
 *
 * // 3. Register the singleton — must be outside the namespace
 * ML_EXPORT(MyTraitDispatcher)
 * @endcode
 *
 * @see Fireable, EventReceiver, ML_EXPORT, Clickable, Hoverable
 */

class EventDispatcher : public Fireable {
private:
    /// Sealed — frame-tick path is not used by EventDispatcher subclasses.
    void fire() final override{};
    /// Sealed — frame-tick path is not used by EventDispatcher subclasses.
    bool occurred() final override{return false;};


public:
    /**
     * @brief Deliver this event to all matching registered components.
     *
     * Called by the framework after @c occurred() returns @c true.
     * Typically iterates the component list and calls @c process() on
     * each component that passes @c filter().
     *
     * @param event The SFML event that triggered this dispatch, or
     *              @c std::nullopt for synthetic/frame-driven events.
     */
    void fire(const std::optional<sf::Event> &event) override = 0;

    /**
     * @brief Return @c true when the incoming SFML event should trigger
     *        this dispatcher.
     *
     * Called once per event by the framework before any components are
     * visited. Return @c false to skip the entire dispatch pass for this
     * event.
     *
     * @param event The SFML event to evaluate, or @c std::nullopt.
     * @return @c true if this dispatcher should fire for @p event.
     */
    bool occurred(const std::optional<sf::Event> &event) override = 0;



};

} // ml

#endif //EVENTDISPATCHER_H