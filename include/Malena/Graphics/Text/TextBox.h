// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef TEXTBOX_H
#define TEXTBOX_H

#pragma once

namespace ml
{
    /**
     * @brief Multi-line text display area. (Currently a stub — not yet implemented.)
      * @ingroup GraphicsText
     *
     * @c TextBox is intended to provide a scrollable, multi-line text
     * display built on top of @c ml::Text, suitable for log output,
     * dialogue boxes, or any UI element that needs to show more text
     * than fits on a single line.
     *
     * The class body is currently empty. For multi-line display now,
     * use @c ml::Text with @c setWordWrap(true) and @c setMaxWidth().
     *
     * @see Text, TextInput
     */
    class TextBox
    {
    };

} // namespace ml

#endif // TEXTBOX_H
