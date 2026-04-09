//
// Event.h
//

#ifndef EVENT_H
#define EVENT_H

#pragma once

#include <Malena/Core/malena_export.h>

namespace ml
{
    /**
     * @brief Framework-level enum for all built-in Malena events.
     * @ingroup EngineEvents
     *
     * All built-in traits use this enum internally. User code can also
     * use these values directly with @c subscribe, @c unsubscribe, and
     * @c publish instead of relying on trait convenience methods.
     *
     * ### Usage
     * @code
     * // Via trait convenience methods (recommended)
     * myRect.onClick([]{ ... });
     * myRect.onHover([]{ ... });
     *
     * // Via raw enum (advanced)
     * myRect.unsubscribe(ml::Event::CLICK);
     * myRect.unsubscribeAll();
     * @endcode
     *
     * @see Clickable, Hoverable, Focusable, Keyable, Updatable, Scrollable,
     *      Selectable
     */
    enum class Event
    {
        // ── Mouse ─────────────────────────────────────────────────────────
        CLICK,           ///< Mouse button released over component
        HOVER,           ///< Mouse entered component bounds
        UNHOVER,         ///< Mouse left component bounds
        MOUSE_PRESSED,   ///< Raw mouse button pressed over component
        MOUSE_RELEASED,  ///< Raw mouse button released over component
        MOUSE_MOVED,     ///< Mouse moved anywhere in window
        DRAG,            ///< Component is being dragged

        // ── Scroll ────────────────────────────────────────────────────────
        SCROLL,          ///< Mouse wheel scrolled over component

        // ── Focus ─────────────────────────────────────────────────────────
        FOCUS,           ///< Component gained keyboard focus
        BLUR,            ///< Component lost keyboard focus

        // ── Keyboard ──────────────────────────────────────────────────────
        KEYPRESS,        ///< Key pressed while component has focus
        KEY_RELEASE,     ///< Key released while component has focus
        TEXT_ENTERED,    ///< Unicode character entered while component has focus

        // ── Selection ─────────────────────────────────────────────────────
        SELECTED,        ///< Component was programmatically selected
        DESELECTED,      ///< Component was programmatically deselected

        // ── Frame ─────────────────────────────────────────────────────────
        UPDATE,          ///< Every frame, before drawing

        // ── Lifecycle ─────────────────────────────────────────────────────
        INIT,            ///< Fired once before the first frame — use to configure properties and register components
        READY,           ///< Fired once after INIT — all components are registered and the object is ready to run

        // ── Window ────────────────────────────────────────────────────────
        WINDOW_RESIZED,       ///< Application window was resized
        WINDOW_FOCUS_GAINED,  ///< Application window gained OS focus
        WINDOW_FOCUS_LOST     ///< Application window lost OS focus
    };

} // namespace ml

#endif // EVENT_H
