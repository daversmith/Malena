// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Event.h
//

#ifndef EVENT_H
#define EVENT_H

#pragma once

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
     * @see Clickable, Hoverable, Focusable, Keyable, Updatable, Scrollable
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

        // ── Frame ─────────────────────────────────────────────────────────
        UPDATE,          ///< Every frame, before drawing

        // ── Window ────────────────────────────────────────────────────────
        WINDOW_RESIZED,       ///< Application window was resized
        WINDOW_FOCUS_GAINED,  ///< Application window gained OS focus
        WINDOW_FOCUS_LOST     ///< Application window lost OS focus
    };

} // namespace ml

#endif // EVENT_H
