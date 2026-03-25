// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 10/5/25.
//

#ifndef SYSTEMSTATESENUM_H
#define SYSTEMSTATESENUM_H

namespace ml
{
    /**
     * @brief System-level boolean flags available on every @c ml::Core object.
     * @ingroup Utilities
     *
     * @c ml::Flag is the enum type managed by @c FlagManager<ml::Flag>, which
     * is inherited by every @c Core object through the @c Flaggable trait.
     * Framework subsystems read and write these flags automatically; user code
     * can query and set them to control built-in behavior.
     *
     * ### Framework-managed flags
     * These are set and cleared by the framework — read them freely, but be
     * careful about writing them manually:
     *
     * | Flag | Set by | Meaning |
     * |------|--------|---------|
     * | @c HOVERED  | @c HoverableDispatcher  | Mouse is currently over this component |
     * | @c CLICKED  | @c ClickableDispatcher  | Mouse button is currently held down over this component |
     * | @c FOCUSED  | @c ClickableDispatcher  | Component has keyboard focus |
     * | @c DRAGGING | @c DraggableDispatcher  | A drag gesture is currently in progress |
     *
     * ### User-controlled flags
     * Set these to enable or configure built-in framework behavior:
     *
     * | Flag | Effect when set |
     * |------|-----------------|
     * | @c DRAGGABLE  | Enables mouse-drag handling in @c ComponentCore |
     * | @c HIDDEN     | Conventionally marks a component as not drawn (implementation-dependent) |
     * | @c BLINKING   | Conventionally marks a component for blink animation |
     * | @c ENABLED    | Conventionally marks a component as active/interactive |
     * | @c VERTICAL   | Hints a layout or scrollable to operate on the vertical axis |
     * | @c HORIZONTAL | Hints a layout or scrollable to operate on the horizontal axis |
     *
     * ### Usage
     * @code
     * // Enable dragging
     * myComponent.setFlag(ml::Flag::DRAGGABLE);
     *
     * // Check hover state
     * if (myComponent.checkFlag(ml::Flag::HOVERED)) { ... }
     *
     * // Hide a component
     * myComponent.setFlag(ml::Flag::HIDDEN);
     * @endcode
     *
     * @note @c LAST_FLAG is a sentinel value used for iteration and array sizing.
     *       Do not use it as an actual flag.
     *
     * @see FlagManager, Flaggable, Draggable
     */
    enum class Flag
    {
        HOVERED,    ///< Mouse is currently over this component (set by HoverableDispatcher).
        CLICKED,    ///< Mouse button is held down over this component (set by ClickableDispatcher).
        HIDDEN,     ///< Component should not be drawn.
        BLINKING,   ///< Component is in a blink animation cycle.
        FOCUSED,    ///< Component has keyboard focus (set by ClickableDispatcher).
        ENABLED,    ///< Component is active and interactive.
        DRAGGABLE,  ///< Enables drag handling in ComponentCore.
        DRAGGING,   ///< A drag gesture is currently in progress (set by DraggableDispatcher).
        VERTICAL,   ///< Layout/scroll axis hint: vertical.
        HORIZONTAL, ///< Layout/scroll axis hint: horizontal.
        LAST_FLAG   ///< Sentinel — do not use as an actual flag value.
    };

} // namespace ml

#endif // SYSTEMSTATESENUM_H
