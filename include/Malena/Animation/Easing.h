// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

//
// Easing.h — pure easing functions for the Animate subsystem.
//

#ifndef MALENA_EASING_H
#define MALENA_EASING_H

#pragma once

#include <cmath>

namespace ml
{
    /**
     * @brief Easing curves for animation, as pure functions of normalized time.
     * @ingroup Animation
     *
     * Every function maps a normalized progress @c t in [0,1] to an eased value.
     * Most return a value in [0,1] (0 at the start, 1 at the end); the @c Back and
     * @c Elastic families intentionally overshoot outside [0,1] for a springy feel.
     *
     * `Easing::Fn` is a plain function pointer, so curves are trivially passed to
     * @c Tweener / @c Animate and cost nothing to store.
     *
     * @code
     * btn.animate().move({400,300}, 0.4f, ml::Easing::EaseOutCubic);
     * @endcode
     */
    struct Easing
    {
        using Fn = float (*)(float);

        static constexpr float kPi = 3.14159265358979323846f;

        // ── Linear ──────────────────────────────────────────────────────────
        static float Linear(float t) { return t; }

        // ── Quadratic ───────────────────────────────────────────────────────
        static float EaseInQuad(float t)    { return t * t; }
        static float EaseOutQuad(float t)   { return 1.f - (1.f - t) * (1.f - t); }
        static float EaseInOutQuad(float t) {
            return t < 0.5f ? 2.f * t * t : 1.f - std::pow(-2.f * t + 2.f, 2.f) / 2.f;
        }

        // ── Cubic ───────────────────────────────────────────────────────────
        static float EaseInCubic(float t)    { return t * t * t; }
        static float EaseOutCubic(float t)   { return 1.f - std::pow(1.f - t, 3.f); }
        static float EaseInOutCubic(float t) {
            return t < 0.5f ? 4.f * t * t * t : 1.f - std::pow(-2.f * t + 2.f, 3.f) / 2.f;
        }

        // ── Quartic ─────────────────────────────────────────────────────────
        static float EaseInQuart(float t)    { return t * t * t * t; }
        static float EaseOutQuart(float t)   { return 1.f - std::pow(1.f - t, 4.f); }
        static float EaseInOutQuart(float t) {
            return t < 0.5f ? 8.f * t * t * t * t : 1.f - std::pow(-2.f * t + 2.f, 4.f) / 2.f;
        }

        // ── Quintic ─────────────────────────────────────────────────────────
        static float EaseInQuint(float t)    { return t * t * t * t * t; }
        static float EaseOutQuint(float t)   { return 1.f - std::pow(1.f - t, 5.f); }
        static float EaseInOutQuint(float t) {
            return t < 0.5f ? 16.f * t * t * t * t * t : 1.f - std::pow(-2.f * t + 2.f, 5.f) / 2.f;
        }

        // ── Sine ────────────────────────────────────────────────────────────
        static float EaseInSine(float t)    { return 1.f - std::cos((t * kPi) / 2.f); }
        static float EaseOutSine(float t)   { return std::sin((t * kPi) / 2.f); }
        static float EaseInOutSine(float t) { return -(std::cos(kPi * t) - 1.f) / 2.f; }

        // ── Exponential ─────────────────────────────────────────────────────
        static float EaseInExpo(float t)  { return t <= 0.f ? 0.f : std::pow(2.f, 10.f * t - 10.f); }
        static float EaseOutExpo(float t) { return t >= 1.f ? 1.f : 1.f - std::pow(2.f, -10.f * t); }
        static float EaseInOutExpo(float t) {
            if (t <= 0.f) return 0.f;
            if (t >= 1.f) return 1.f;
            return t < 0.5f ? std::pow(2.f, 20.f * t - 10.f) / 2.f
                            : (2.f - std::pow(2.f, -20.f * t + 10.f)) / 2.f;
        }

        // ── Circular ────────────────────────────────────────────────────────
        static float EaseInCirc(float t)  { return 1.f - std::sqrt(1.f - t * t); }
        static float EaseOutCirc(float t) { return std::sqrt(1.f - (t - 1.f) * (t - 1.f)); }
        static float EaseInOutCirc(float t) {
            return t < 0.5f
                ? (1.f - std::sqrt(1.f - std::pow(2.f * t, 2.f))) / 2.f
                : (std::sqrt(1.f - std::pow(-2.f * t + 2.f, 2.f)) + 1.f) / 2.f;
        }

        // ── Back (overshoots) ───────────────────────────────────────────────
        static float EaseInBack(float t) {
            const float c1 = 1.70158f, c3 = c1 + 1.f;
            return c3 * t * t * t - c1 * t * t;
        }
        static float EaseOutBack(float t) {
            const float c1 = 1.70158f, c3 = c1 + 1.f;
            return 1.f + c3 * std::pow(t - 1.f, 3.f) + c1 * std::pow(t - 1.f, 2.f);
        }
        static float EaseInOutBack(float t) {
            const float c1 = 1.70158f, c2 = c1 * 1.525f;
            return t < 0.5f
                ? (std::pow(2.f * t, 2.f) * ((c2 + 1.f) * 2.f * t - c2)) / 2.f
                : (std::pow(2.f * t - 2.f, 2.f) * ((c2 + 1.f) * (t * 2.f - 2.f) + c2) + 2.f) / 2.f;
        }

        // ── Elastic (springs) ───────────────────────────────────────────────
        static float EaseInElastic(float t) {
            if (t <= 0.f) return 0.f;
            if (t >= 1.f) return 1.f;
            const float c4 = (2.f * kPi) / 3.f;
            return -std::pow(2.f, 10.f * t - 10.f) * std::sin((t * 10.f - 10.75f) * c4);
        }
        static float EaseOutElastic(float t) {
            if (t <= 0.f) return 0.f;
            if (t >= 1.f) return 1.f;
            const float c4 = (2.f * kPi) / 3.f;
            return std::pow(2.f, -10.f * t) * std::sin((t * 10.f - 0.75f) * c4) + 1.f;
        }
        static float EaseInOutElastic(float t) {
            if (t <= 0.f) return 0.f;
            if (t >= 1.f) return 1.f;
            const float c5 = (2.f * kPi) / 4.5f;
            return t < 0.5f
                ? -(std::pow(2.f, 20.f * t - 10.f) * std::sin((20.f * t - 11.125f) * c5)) / 2.f
                :  (std::pow(2.f, -20.f * t + 10.f) * std::sin((20.f * t - 11.125f) * c5)) / 2.f + 1.f;
        }

        // ── Bounce ──────────────────────────────────────────────────────────
        static float EaseOutBounce(float t) {
            const float n1 = 7.5625f, d1 = 2.75f;
            if (t < 1.f / d1)       return n1 * t * t;
            else if (t < 2.f / d1) { t -= 1.5f  / d1; return n1 * t * t + 0.75f; }
            else if (t < 2.5f / d1){ t -= 2.25f / d1; return n1 * t * t + 0.9375f; }
            else                   { t -= 2.625f/ d1; return n1 * t * t + 0.984375f; }
        }
        static float EaseInBounce(float t)  { return 1.f - EaseOutBounce(1.f - t); }
        static float EaseInOutBounce(float t) {
            return t < 0.5f
                ? (1.f - EaseOutBounce(1.f - 2.f * t)) / 2.f
                : (1.f + EaseOutBounce(2.f * t - 1.f)) / 2.f;
        }
    };

} // namespace ml

#endif // MALENA_EASING_H
