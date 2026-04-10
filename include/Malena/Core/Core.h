// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 10/4/25.
//

#ifndef MALENA_UICOMPONENTBASE_H
#define MALENA_UICOMPONENTBASE_H

#include <Malena/Core/malena_export.h>
#include <SFML/Graphics.hpp>

#include <Malena/Traits/Communication/Subscribable.h>
#include <Malena/Traits/Interaction/Draggable.h>
#include <Malena/Traits/Spatial/Positionable.h>
#include <Malena/Traits/State/Flaggable.h>

#include <Malena/Traits/Interaction/Keyable.h>
#include <Malena/Traits/Interaction/Scrollable.h>
#include <Malena/Traits/Lifecycle/Updatable.h>
#include <Malena/Traits/Interaction/Clickable.h>
#include <Malena/Traits/Interaction/Focusable.h>
#include <Malena/Traits/Interaction/Hoverable.h>

#include <Malena/Traits/Communication/Unsubscribable.h>

namespace ml
{
    /**
     * @brief Virtual base class for all Malena framework objects.
      * @ingroup Core
     *
     * @c Core combines the fundamental traits that every framework object
     * is expected to support:
     * - @c Subscribable — can subscribe to and publish framework events
     * - @c Flaggable    — carries system-level boolean flags (@c ml::Flag)
     * - @c Positionable — has a position, global bounds, and spatial identity
     * - @c Clickable, @c Hoverable, @c Focusable — pointer interaction
     * - @c Keyable, @c Scrollable — keyboard and scroll-wheel input
     * - @c Updatable    — receives a per-frame update callback
     * - @c Unsubscribable — can remove all its event subscriptions at once
     *
     * All inheritance is @c virtual so that the diamond formed by
     * @c ComponentCore and its trait hierarchy collapses to a single
     * instance of each base.
     *
     * @c Core does not inherit @c sf::Drawable. Drawable behavior is added
     * by @c ComponentBase (for standard components) or by the shape type @c T
     * in @c Graphic (for graphics primitives).
     *
     * @note Direct instantiation is not intended. Inherit through
     *       @c ml::Component, @c ml::Graphic, or a plugin base.
     *
     * @see Component, CoreAdapter, ComponentCore
     */
    class MALENA_API Core : public virtual Subscribable,
                 public virtual Flaggable,
                 public virtual Positionable,
                 public virtual Clickable,
				 public virtual Hoverable,
				 public virtual Focusable,
                 public virtual Keyable,
                 public virtual Scrollable,
                 // public virtual Draggable,
                 public virtual Updatable,
				 public Unsubscribable

    {
    public:
        virtual ~Core();

        /**
         * @brief Returns the SFML render states to use when drawing this object.
         *
         * The default implementation returns a default-constructed
         * @c sf::RenderStates (no transform, no texture, no blend mode override).
         * Override to apply a custom shader, blend mode, or transform.
         *
         * @return sf::RenderStates to pass to the SFML draw call.
         */
        virtual sf::RenderStates getRenderStates() const { return sf::RenderStates(); }
    };

} // namespace ml

#endif // MALENA_UICOMPONENTBASE_H
