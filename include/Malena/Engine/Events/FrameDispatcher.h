// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 3/22/26.
//

#ifndef MALENA_FRAMEDISPATCHER_H
#define MALENA_FRAMEDISPATCHER_H
#include <Malena/Core/malena_export.h>
#include <Malena/Engine/Events/Fireable.h>
#include <optional>

namespace ml {

/**
 * @brief Base class for per-frame dispatchers in the Malena event system.
 * @ingroup EngineEvents
 *
 * @c FrameDispatcher is the frame-tick counterpart to @c EventDispatcher.
 * Where @c EventDispatcher responds to SFML input events (mouse clicks,
 * key presses, etc.), @c FrameDispatcher fires unconditionally once per
 * frame — before drawing — making it the correct base for traits like
 * @c Updatable that need to run every tick regardless of user input.
 *
 * The constructor automatically passes @c DispatchType::FRAME to
 * @c Fireable, so the framework routes this dispatcher through the
 * per-frame loop rather than the per-event loop. No manual setup is needed.
 *
 * The SFML-event–taking overloads inherited from @c Fireable are sealed
 * @c final so you only need to implement the two no-argument versions.
 *
 * ### How it fits together
 *
 * The same three-piece pattern as @c EventDispatcher applies:
 *
 * 1. **A receiver class** — inherits @c EventReceiver and exposes a friendly
 *    callback registration method that stores a callback via
 *    @c Fireable::addCallback.
 *
 * 2. **A dispatcher class** — inherits @c FrameDispatcher and implements
 *    @c occurred() and @c fire(). Registered as a singleton via @c ML_EXPORT.
 *
 * 3. **@c ML_EXPORT(MyDispatcher)** — placed after the class definition.
 *
 * ### What each method does
 *
 * | Method | Purpose |
 * |--------|---------|
 * | @c occurred() | Return @c true when this dispatcher should fire this frame. Return @c false to skip the pass entirely. Typically always returns @c true for unconditional per-frame traits. |
 * | @c fire() | Iterate registered components and deliver the per-frame callback. |
 *
 * ### Minimal example
 * @code
 * // ── MyFrameTrait.h ────────────────────────────────────────────────────────
 *
 * // 1. Receiver
 * class MyFrameTrait : public ml::EventReceiver
 * {
 * public:
 *     void onTick(std::function<void()> f)
 *     {
 *         Fireable::addCallback(ml::Event::UPDATE, this,
 *             [f](const std::optional<sf::Event>&){ f(); });
 *     }
 * };
 *
 * // 2. Dispatcher
 * class MyFrameDispatcher : public ml::FrameDispatcher
 * {
 * public:
 *     bool occurred() override
 *     {
 *         return true; // fire every frame
 *     }
 *
 *     void fire() override
 *     {
 *         ml::EventManager::fire(ml::Event::UPDATE, this, std::nullopt);
 *     }
 * };
 *
 * // 3. Register the singleton
 * ML_EXPORT(MyFrameDispatcher)
 * @endcode
 *
 * @see Fireable, EventDispatcher, EventReceiver, Updatable, ML_EXPORT
 */
class MALENA_API FrameDispatcher : public Fireable {
public:
    FrameDispatcher() : Fireable(DispatchType::FRAME) {}

    /**
     * @brief Return @c true when this dispatcher should fire this frame.
     *
     * Called once per frame by the framework. Return @c false to skip
     * the entire delivery pass for this frame. Most per-frame traits
     * simply return @c true unconditionally.
     *
     * @return @c true if @c fire() should be called this frame.
     */
    bool occurred() override = 0;

    /**
     * @brief Deliver the per-frame callback to all matching components.
     *
     * Called by the framework each frame after @c occurred() returns
     * @c true. Typically iterates the component list and calls
     * @c process() on each relevant component.
     */
    void fire() override = 0;

private:
    /// Sealed — SFML-event path is not used by FrameDispatcher subclasses.
    void fire(const std::optional<sf::Event>& event) final override {}
    /// Sealed — SFML-event path is not used by FrameDispatcher subclasses.
    bool occurred(const std::optional<sf::Event>& event) final override { return false; }
};

} // namespace ml



#endif //FRAMEDISPATCHER_H