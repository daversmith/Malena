// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

//
// Animate.h — per-object animation facade.
//
// Obtained via Positionable::animate(). Drives the owner's position (and any
// app-supplied property) through the shared AnimationManager, using delta-time
// Tweeners and Easing curves. Replaces the old moveTo/moveDistance code.
//

#ifndef MALENA_ANIMATE_H
#define MALENA_ANIMATE_H

#pragma once

#include <Malena/Animation/AnimationManager.h>
#include <Malena/Animation/Easing.h>
#include <Malena/Animation/Tweener.h>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <memory>

namespace ml
{
    class Positionable;

    /**
     * @brief Fluent animation controller bound to one object.
     * @ingroup Animation
     *
     * @code
     * comp.animate().move({400.f, 300.f}, 0.4f, Easing::EaseOutCubic);
     * comp.animate().moveBy({0.f, -20.f}, 0.2f);
     * comp.animate().value<sf::Color>(from, to, 0.3f, Easing::Linear,
     *     [&](const sf::Color& c){ shape.setFillColor(c); });
     * @endcode
     *
     * A new @c move / @c moveBy cancels this object's previous animations first,
     * so position tweens never fight. Animations are automatically cancelled when
     * the owning object is destroyed.
     */
    class MALENA_API Animate
    {
    public:
        explicit Animate(Positionable* owner) : _owner(owner) {}
        ~Animate() { cancel(); }

        Animate(const Animate&)            = delete;
        Animate& operator=(const Animate&) = delete;

        /// Animate the owner's position to @p to over @p seconds.
        Animate& move(sf::Vector2f to, float seconds,
                      Easing::Fn ease = Easing::EaseOutCubic,
                      std::function<void()> onComplete = {});

        /// Animate the owner's position by a relative @p delta over @p seconds.
        Animate& moveBy(sf::Vector2f delta, float seconds,
                        Easing::Fn ease = Easing::EaseOutCubic,
                        std::function<void()> onComplete = {});

        /**
         * @brief Animate an arbitrary value and deliver it through @p apply.
         *
         * The engine is type-generic (float / sf::Vector2f / sf::Color); the
         * caller supplies the setter, so this drives color, scale, opacity, scroll
         * — anything lerpable — without Animate needing to know the property.
         */
        template<class T>
        Animate& value(T from, T to, float seconds, Easing::Fn ease,
                       std::function<void(const T&)> apply,
                       std::function<void()> onComplete = {})
        {
            auto tw = std::make_shared<Tweener<T>>(from, to, seconds, ease);
            tw->onUpdate(std::move(apply));
            if (onComplete) tw->onComplete(std::move(onComplete));
            AnimationManager::add(_owner, [tw](float dt) { tw->update(dt); return tw->done(); });
            return *this;
        }

        /// Cancel every animation on this object (no completion callbacks fire).
        void cancel();

        /// True while any animation on this object is running.
        [[nodiscard]] bool active() const;

    private:
        Positionable* _owner;
    };

} // namespace ml

#endif // MALENA_ANIMATE_H
