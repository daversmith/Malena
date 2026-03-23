#pragma once
// ============================================================
//  include/Malena/Traits/Fadeable.h
// ============================================================
//
//  @brief Trait that adds time-based alpha animation to any component.
//
//  DESIGN NOTES
//  ────────────
//  Fadeable follows the same pattern as Draggable and Messenger:
//    - It is a Trait subclass with its own FadeableManifest
//    - It owns its animation state internally
//    - It subscribes itself to the "update" event via EventsManager
//      in its constructor, so the tick loop is automatic — no manual
//      onUpdate() call needed in the component
//    - The component reads getAlpha() in its draw() method and applies
//      it to whatever it is drawing
//
//  USAGE
//  ─────
//  1. Add Fadeable as a trait to your component:
//
//     class MyComp : public ml::Component<ml::Fadeable> { ... };
//     // or with Messenger too:
//     class MyComp : public ml::Component<ml::Fadeable, ml::Messenger> { ... };
//
//  2. Call fade methods anywhere — constructor, event callbacks, etc.:
//
//     fadeIn(0.5f);                          // fade from 0 → 255 over 0.5s
//     fadeOut(1.0f, ml::EXPONENTIAL);        // fade 255 → 0 with ease-out
//     fadeTo(128, 0.3f);                     // fade to half-opacity
//     hold(2.0f).then([this]{ fadeOut(); }); // hold then chain
//
//  3. Apply alpha in your draw() override:
//
//     void draw(sf::RenderTarget& t, const sf::RenderStates& s) const override
//     {
//         sf::Color c = _body.getFillColor();
//         c.a = getAlpha();
//         _body.setFillColor(c);
//         t.draw(_body, s);
//     }
//
// ============================================================

#include <Malena/Traits/Base/Trait.h>
#include <Malena/Engine/Events/EventsManager.h>
#include <Malena/Utilities/Tween.h>
#include <functional>
#include <optional>
#include <cstdint>

namespace ml
{
    /**
     * @brief Trait that adds time-based alpha animation to any component.
     * @ingroup Traits
     *
     * Fadeable manages a float alpha value (0.0–255.0) and animates it
     * toward a target over a specified duration using linear or exponential
     * easing. The animation is driven by the framework's "update" event
     * and is therefore tied to real elapsed time rather than frame counts.
     *
     * An optional completion callback fires exactly once when the animation
     * reaches its target, enabling chained sequences like:
     * @code
     * fadeIn(0.3f, ml::LINEAR, [this]{ hold(2.f, [this]{ fadeOut(0.5f); }); });
     * @endcode
     *
     * @see Draggable, Positionable, Tween
     */
    class Fadeable : public Trait
    {
    public:
        Fadeable();
        virtual ~Fadeable();

        // ── Fade API ──────────────────────────────────────────────────────────

        /**
         * @brief Animate alpha from its current value to @p targetAlpha.
         *
         * @param targetAlpha  Target alpha (0–255).
         * @param duration     Animation duration in seconds. Defaults to 0.5s.
         * @param tween        Easing curve. Defaults to LINEAR.
         * @param onComplete   Optional callback fired when the animation ends.
         */
        void fadeTo(uint8_t                  targetAlpha,
                    float                    duration   = 0.5f,
                    Tween                    tween      = LINEAR,
                    std::function<void()>    onComplete = nullptr);

        /**
         * @brief Animate alpha from 0 to 255 (fully opaque).
         * Shorthand for fadeTo(255, duration, tween, onComplete).
         */
        void fadeIn(float                 duration   = 0.5f,
                    Tween                 tween      = LINEAR,
                    std::function<void()> onComplete = nullptr);

        /**
         * @brief Animate alpha from current value to 0 (fully transparent).
         * Shorthand for fadeTo(0, duration, tween, onComplete).
         */
        void fadeOut(float                 duration   = 0.5f,
                     Tween                 tween      = LINEAR,
                     std::function<void()> onComplete = nullptr);

        /**
         * @brief Hold at the current alpha for @p duration seconds,
         *        then fire @p onComplete.
         *
         * Useful for sequencing: fade in → hold → fade out.
         * @code
         * fadeIn(0.3f, LINEAR, [this]{
         *     holdFor(2.f, [this]{ fadeOut(0.5f); });
         * });
         * @endcode
         */
        void holdFor(float                 duration,
                     std::function<void()> onComplete = nullptr);

        /**
         * @brief Set alpha immediately with no animation.
         * Cancels any animation in progress.
         */
        void setAlpha(uint8_t alpha);

        /**
         * @brief Return the current alpha as a uint8_t (0–255).
         * Call this in your draw() method to apply the animated value.
         */
        uint8_t getAlpha() const;

        /**
         * @brief Return true while a fade or hold animation is running.
         */
        bool isFading() const;

    private:
        // ── Internal tick ─────────────────────────────────────────────────────
        //  Called every frame via the "update" event subscription.
        //  dt is approximated as 1/60 — a future version can pass real dt
        //  once the framework exposes it through the update event payload.
        void tick();

        // ── Animation state ───────────────────────────────────────────────────
        float  _alpha        = 255.f;  // current alpha as float for smooth steps
        float  _startAlpha   = 255.f;  // alpha at animation start
        float  _targetAlpha  = 255.f;  // animation destination
        float  _elapsed      = 0.f;    // time elapsed in current animation
        float  _duration     = 0.f;    // total duration of current animation
        bool   _active       = false;  // true while animating or holding
        bool   _holding      = false;  // true during a holdFor()
        Tween  _tween        = LINEAR;

        std::function<void()> _onComplete;  // fired once when animation ends

        // ── Easing helpers ────────────────────────────────────────────────────
        static float applyTween(float t, Tween tween);
    };

} // namespace ml

#include "Fadeable.tpp"
