// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#pragma once

#include <Malena/Traits/Base/Trait.h>
#include <Malena/Engine/Events/EventManager.h>
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
     * @c Fadeable manages a float alpha value (0.0–255.0) and animates it
     * toward a target over a specified duration using linear or exponential
     * easing. The animation tick is driven automatically by the framework's
     * update event — no manual @c onUpdate call is needed in the component.
     *
     * An optional completion callback fires exactly once when the animation
     * reaches its target, enabling chained sequences:
     * @code
     * fadeIn(0.3f, ml::LINEAR, [this]{
     *     holdFor(2.f, [this]{ fadeOut(0.5f); });
     * });
     * @endcode
     *
     * ### Adding Fadeable to a component
     * @code
     * class MyComp : public ml::Component<ml::Fadeable> { ... };
     * // Or combined with other traits:
     * class MyComp : public ml::Component<ml::Fadeable, ml::Messenger> { ... };
     * @endcode
     *
     * ### Applying the alpha in draw()
     * @c Fadeable does not modify any drawable automatically — call @c getAlpha()
     * in your @c draw() override and apply the value to whatever you are drawing:
     * @code
     * void draw(sf::RenderTarget& t, const sf::RenderStates& s) const override
     * {
     *     sf::Color c = _body.getFillColor();
     *     c.a = getAlpha();
     *     _body.setFillColor(c);
     *     t.draw(_body, s);
     * }
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
         * @param tween        Easing curve. Defaults to @c LINEAR.
         * @param onComplete   Optional callback fired once when the animation ends.
         */
        void fadeTo(uint8_t               targetAlpha,
                    float                 duration   = 0.5f,
                    Tween                 tween      = LINEAR,
                    std::function<void()> onComplete = nullptr);

        /**
         * @brief Animate alpha from 0 to 255 (fully opaque).
         *
         * Shorthand for @c fadeTo(255, duration, tween, onComplete).
         *
         * @param duration     Animation duration in seconds. Defaults to 0.5s.
         * @param tween        Easing curve. Defaults to @c LINEAR.
         * @param onComplete   Optional callback fired once when the animation ends.
         */
        void fadeIn(float                 duration   = 0.5f,
                    Tween                 tween      = LINEAR,
                    std::function<void()> onComplete = nullptr);

        /**
         * @brief Animate alpha from its current value to 0 (fully transparent).
         *
         * Shorthand for @c fadeTo(0, duration, tween, onComplete).
         *
         * @param duration     Animation duration in seconds. Defaults to 0.5s.
         * @param tween        Easing curve. Defaults to @c LINEAR.
         * @param onComplete   Optional callback fired once when the animation ends.
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
         *
         * @param duration     Hold duration in seconds.
         * @param onComplete   Optional callback fired once the hold period ends.
         */
        void holdFor(float                 duration,
                     std::function<void()> onComplete = nullptr);

        /**
         * @brief Set alpha immediately with no animation.
         *
         * Cancels any animation in progress.
         *
         * @param alpha The new alpha value (0–255).
         */
        void setAlpha(uint8_t alpha);

        /**
         * @brief Return the current alpha as a @c uint8_t (0–255).
         *
         * Call this in your @c draw() override to apply the animated value
         * to your drawable's color.
         *
         * @return Current alpha value.
         */
        uint8_t getAlpha() const;

        /**
         * @brief Return @c true while a fade or hold animation is running.
         *
         * @return @c true if an animation or hold is currently in progress.
         */
        bool isFading() const;

    private:
        /// @cond INTERNAL
        void tick();

        float  _alpha       = 255.f;
        float  _startAlpha  = 255.f;
        float  _targetAlpha = 255.f;
        float  _elapsed     = 0.f;
        float  _duration    = 0.f;
        bool   _active      = false;
        bool   _holding     = false;
        Tween  _tween       = LINEAR;

        std::function<void()> _onComplete;

        static float applyTween(float t, Tween tween);
        /// @endcond
    };

} // namespace ml

#include "../../../src/Traits/Fadeable.tpp"
