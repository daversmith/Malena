// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

//
// Tweener.h — a generic, delta-time value animator.
//
// Interpolates any lerpable value (float, sf::Vector2f, sf::Color) from a start
// to an end over a duration, using an Easing curve, advanced by REAL elapsed
// time. This is the stable replacement for the old frame-count-based movement
// code that used to live in Positionable.
//

#ifndef MALENA_TWEENER_H
#define MALENA_TWEENER_H

#pragma once

#include <Malena/Animation/Easing.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <functional>

namespace ml
{
    namespace detail
    {
        // ── lerp overloads — one per interpolatable type ─────────────────────
        inline float lerp(float a, float b, float t) { return a + (b - a) * t; }

        inline sf::Vector2f lerp(const sf::Vector2f& a, const sf::Vector2f& b, float t)
        {
            return { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t };
        }

        inline sf::Color lerp(const sf::Color& a, const sf::Color& b, float t)
        {
            auto ch = [t](std::uint8_t x, std::uint8_t y) -> std::uint8_t {
                const float v = static_cast<float>(x) + (static_cast<float>(y) - x) * t;
                return static_cast<std::uint8_t>(std::clamp(v, 0.f, 255.f) + 0.5f);
            };
            return { ch(a.r, b.r), ch(a.g, b.g), ch(a.b, b.b), ch(a.a, b.a) };
        }
    } // namespace detail

    /**
     * @brief A single delta-time value animation.
     * @ingroup Animation
     *
     * Drive it by calling @c update(dt) each frame with the elapsed seconds. The
     * interpolated value is delivered through @c onUpdate; @c onComplete fires
     * once when a non-looping tween finishes. Timing is real-time (seconds), so
     * animation speed is independent of framerate.
     *
     * @code
     * Tweener<sf::Vector2f> t(start, end, 0.4f, Easing::EaseOutCubic);
     * t.onUpdate([&](const sf::Vector2f& p){ sprite.setPosition(p); });
     * // each frame:
     * t.update(dt);
     * @endcode
     *
     * @tparam T A type with a @c detail::lerp overload (float, Vector2f, Color).
     */
    template<class T>
    class Tweener
    {
    public:
        Tweener() = default;

        Tweener(T from, T to, float duration, Easing::Fn easing = Easing::Linear)
            : _from(from), _to(to), _value(from),
              _duration(duration > 0.f ? duration : 0.f), _easing(easing ? easing : Easing::Linear)
        {}

        // ── Fluent configuration ─────────────────────────────────────────────
        Tweener& delay(float seconds)                       { _delay = std::max(0.f, seconds); return *this; }
        Tweener& easing(Easing::Fn fn)                      { _easing = fn ? fn : Easing::Linear; return *this; }
        Tweener& loop(bool on = true)                       { _loop = on; return *this; }
        Tweener& pingPong(bool on = true)                   { _pingPong = on; return *this; }
        Tweener& onUpdate(std::function<void(const T&)> cb) { _onUpdate = std::move(cb); return *this; }
        Tweener& onComplete(std::function<void()> cb)       { _onComplete = std::move(cb); return *this; }

        // ── Drive ─────────────────────────────────────────────────────────────
        /// Advance by @p dt seconds. Safe to keep calling after completion (no-op).
        void update(float dt)
        {
            if (_done) return;
            if (dt < 0.f) dt = 0.f;

            // Hold at the start value through the delay window.
            if (_delay > 0.f)
            {
                _delay -= dt;
                if (_delay > 0.f) { emit(_from); return; }
                dt = -_delay;       // spend the leftover on the tween
                _delay = 0.f;
            }

            // Zero-duration tween snaps straight to the end.
            if (_duration <= 0.f)
            {
                _value = _to;
                emit(_value);
                finish();
                return;
            }

            _elapsed += dt;
            const float eased = _easing(phase());
            _value = detail::lerp(_from, _to, eased);
            emit(_value);

            if (!_loop && !_pingPong && _elapsed >= _duration)      finish();
            else if (_pingPong && !_loop && _elapsed >= 2.f * _duration) finish();
        }

        /// Jump to the end immediately, firing onUpdate(end) + onComplete once.
        void complete()
        {
            if (_done) return;
            _value = _to;
            emit(_value);
            finish();
        }

        /// Stop where it is — no onComplete. (Used by cancel paths.)
        void stop() { _done = true; }

        [[nodiscard]] bool done()  const { return _done; }
        [[nodiscard]] const T& value() const { return _value; }

    private:
        // Normalized eased-input in [0,1], honoring loop / ping-pong.
        [[nodiscard]] float phase() const
        {
            if (_duration <= 0.f) return 1.f;
            float p = _elapsed / _duration;                 // 0..∞ in "durations"
            if (_pingPong)
            {
                float c = std::fmod(p, 2.f);                // 0..2 triangle
                if (!_loop) c = std::min(p, 2.f);           // one there-and-back
                return c <= 1.f ? c : 2.f - c;
            }
            if (_loop) return std::fmod(p, 1.f);
            return std::clamp(p, 0.f, 1.f);
        }

        void emit(const T& v) const { if (_onUpdate) _onUpdate(v); }

        void finish()
        {
            _done = true;
            if (_onComplete) _onComplete();
        }

        T           _from{}, _to{}, _value{};
        float       _duration = 0.f;
        float       _delay    = 0.f;
        float       _elapsed  = 0.f;
        Easing::Fn  _easing   = Easing::Linear;
        bool        _loop     = false;
        bool        _pingPong = false;
        bool        _done     = false;
        std::function<void(const T&)> _onUpdate;
        std::function<void()>         _onComplete;
    };

} // namespace ml

#endif // MALENA_TWEENER_H
