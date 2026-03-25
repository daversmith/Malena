//
// Created by Dave Smith on 10/4/25.
//

#ifndef UICOMPONENTBASE_H
#define UICOMPONENTBASE_H

#include <SFML/Graphics.hpp>

#include <Malena/Engine/Window/WindowManager.h>
#include <Malena/Traits/Draggable.h>
#include <Malena/Traits/Flaggable.h>
#include <Malena/Traits/Messenger.h>
#include <Malena/Traits/Positionable.h>
#include <Malena/Traits/Subscribable.h>
#include <Malena/Utilities/MouseEvents.h>
#include <SFML/Graphics/Sprite.hpp>
#include <iostream>
#include <list>

#include <Malena/Traits/Keyable.h>
#include <Malena/Traits/Scrollable.h>
#include <Malena/Traits/Updatable.h>
#include "Malena/Traits/Clickable.h"
#include "Malena/Traits/Focusable.h"
#include "Malena/Traits/Hoverable.h"

#include "../Traits/Unsubscribable.h"

namespace ml
{
    /**
     * @brief Virtual base class for all Malena framework objects.
      * @ingroup Core
     *
     * @c Core combines the three fundamental traits that every framework
     * object is expected to support:
     * - @c Subscribable — can subscribe to and receive framework events
     * - @c Flaggable    — carries a set of system-level boolean flags (ml::Flag)
     * - @c Positionable — has a position, global bounds, and spatial identity
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
    class Core : public virtual Subscribable,
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

#endif // UICOMPONENTBASE_H
