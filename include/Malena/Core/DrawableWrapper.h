// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 3/14/26.
//

#ifndef MALENA_DRAWABLEWRAPPER_H
#define MALENA_DRAWABLEWRAPPER_H

#include <Malena/Core/malena_export.h>

namespace ml
{
    // ── Detection traits ─────────────────────────────────────────────────────

    /// @cond INTERNAL

    /** @brief Detects whether @c T has a @c setPosition(sf::Vector2f) method. */
    template<typename T, typename = void>
    struct HasSetPosition : std::false_type {};
    template<typename T>
    struct HasSetPosition<T, std::void_t<
        decltype(std::declval<T>().setPosition(std::declval<sf::Vector2f>()))
    >> : std::true_type {};

    /** @brief Detects whether @c T has a @c getPosition() method. */
    template<typename T, typename = void>
    struct HasGetPosition : std::false_type {};
    template<typename T>
    struct HasGetPosition<T, std::void_t<
        decltype(std::declval<T>().getPosition())
    >> : std::true_type {};

    /** @brief Detects whether @c T has a @c getGlobalBounds() method. */
    template<typename T, typename = void>
    struct HasGetGlobalBounds : std::false_type {};
    template<typename T>
    struct HasGetGlobalBounds<T, std::void_t<
        decltype(std::declval<T>().getGlobalBounds())
    >> : std::true_type {};

    /// @endcond

    // ── DrawableWrapper ───────────────────────────────────────────────────────

    /**
     * @brief Adapts any @c sf::Drawable into a first-class @c ml::Core object.
     * @ingroup Core
     *
     * @c DrawableWrapper<ENTITY> inherits from both @c ENTITY and @c ml::Core,
     * bridging the gap between raw SFML drawables (which know nothing about
     * the Malena framework) and the framework's component and plugin systems.
     *
     * Position and bounds queries are forwarded to @c ENTITY using compile-time
     * detection:
     * - If @c ENTITY provides @c setPosition / @c getPosition, they are delegated.
     * - If @c ENTITY provides @c getGlobalBounds, it is delegated.
     * - If @c ENTITY has no @c getGlobalBounds, the fallback returns the full
     *   window rect so the object is never accidentally excluded from hit-testing.
     *
     * All constructors of @c ENTITY are forwarded via @c using @c ENTITY::ENTITY,
     * so @c DrawableWrapper can be constructed exactly like the wrapped type.
     *
     * @tparam ENTITY An @c sf::Drawable-derived type to wrap.
     *
     * @see Core, CoreManager, CoreAdapter
     */
    template<typename ENTITY>
    class DrawableWrapper : public ENTITY, public ml::Core
    {
    public:
        using ENTITY::ENTITY;

        /**
         * @brief Set the world-space position, delegating to @c ENTITY if supported.
         *
         * If @c ENTITY does not provide @c setPosition(sf::Vector2f), this
         * call is a no-op (no compile error).
         *
         * @param pos New position in world coordinates.
         */
        void setPosition(const sf::Vector2f& pos) override
        {
            if constexpr (HasSetPosition<ENTITY>::value)
                ENTITY::setPosition(pos);
        }

        /**
         * @brief Return the world-space position, delegating to @c ENTITY if supported.
         *
         * If @c ENTITY does not provide @c getPosition(), returns the zero vector.
         *
         * @return Current position, or @c {0, 0} if @c ENTITY has no position.
         */
        sf::Vector2f getPosition() const override
        {
            if constexpr (HasGetPosition<ENTITY>::value)
                return ENTITY::getPosition();
            return {};
        }

        /**
         * @brief Return the axis-aligned bounding box, delegating to @c ENTITY if supported.
         *
         * If @c ENTITY does not provide @c getGlobalBounds(), falls back to
         * the full window rectangle so that the wrapped object is never
         * inadvertently excluded from hit-testing or culling.
         *
         * @return Bounding rect in world coordinates, or the full window rect
         *         if @c ENTITY has no bounds.
         */
        sf::FloatRect getGlobalBounds() const override
        {
            if constexpr (HasGetGlobalBounds<ENTITY>::value)
                return ENTITY::getGlobalBounds();
            auto size = ml::WindowManager::getWindow().getSize();
            return sf::FloatRect({0.f, 0.f}, sf::Vector2f(size));
        }
    };

} // namespace ml

#endif // MALENA_DRAWABLEWRAPPER_H
