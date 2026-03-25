// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef UICOMPONENTADAPTER_H
#define UICOMPONENTADAPTER_H

#pragma once

#include <Malena/Core/Core.h>
#include <optional>
namespace ml
{
    /**
     * @brief Concrete adapter that gives @c Core a default virtual interface.
      * @ingroup Core
     *
     * @c CoreAdapter extends @c Core with virtual implementations of the
     * methods that SFML and the framework expect from a drawable, updatable
     * object — without committing to a specific shape or rendering strategy.
     *
     * Use @c CoreAdapter as a base when you need a framework object that:
     * - is not a template (unlike @c Component<Manifest>)
     * - needs to override draw, update, or event handling individually
     * - participates in the @c CoreManager collection without a manifest
     *
     * All methods have empty default implementations, so subclasses override
     * only what they need.
     *
     * @note Most user-facing components should prefer @c ml::Component or
     *       @c ml::ComponentWith, which layer manifest support, flag
     *       gathering, and SFML drawable registration automatically.
     *       @c CoreAdapter is primarily useful for framework-internal types
     *       and advanced custom objects.
     *
     * @see Core, Component, CoreManager
     */
    class CoreAdapter : public Core
    {
    protected:
        /**
         * @brief Draw this object to an SFML render target.
         *
         * Default implementation does nothing. Override to render
         * custom geometry or delegate to an internal SFML drawable.
         *
         * @param target The SFML render target (window or texture).
         * @param states The current render states (transform, shader, etc.).
         */
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    public:
        /**
         * @brief Perform per-frame logic update.
         *
         * Called by the manager each frame. Default implementation does nothing.
         */
        virtual void update();

        /**
         * @brief Handle an incoming SFML event.
         *
         * Called by the manager for each polled SFML event. Default
         * implementation does nothing.
         *
         * @param event The SFML event, wrapped in @c std::optional.
         */
        virtual void eventHandler(const std::optional<sf::Event>& event);

        /**
         * @brief Return the axis-aligned bounding box of this object in world space.
         *
         * Used by the framework for hit-testing (click, hover). Default
         * implementation returns an empty rect at the origin. Override to
         * return meaningful bounds.
         *
         * @return Bounding rectangle in world coordinates.
         */
        virtual sf::FloatRect getGlobalBounds() const override;

        /**
         * @brief Set the world-space position of this object.
         *
         * @param position New position in world coordinates.
         */
        virtual void setPosition(const sf::Vector2f& position) override;

        /**
         * @brief Return the current world-space position of this object.
         *
         * @return Current position in world coordinates.
         */
        virtual sf::Vector2f getPosition() const override;
    };

} // namespace ml

#endif // UICOMPONENTADAPTER_H
