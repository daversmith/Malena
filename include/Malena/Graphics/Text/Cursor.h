// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef CURSOR_H
#define CURSOR_H

#include <Malena/Resources/FontManager.h>
#include <Malena/Graphics/Text/Text.h>

namespace ml
{
    /**
     * @brief A blinking text cursor used inside @c TextInput.
      * @ingroup GraphicsText
     *
     * @c Cursor extends @c ml::Text with a time-driven blink animation,
     * rendering a visible caret character that alternates between visible
     * and invisible on a fixed interval driven by a static @c sf::Clock.
     *
     * The @c setString method is hidden from the public API — cursor
     * content is managed internally and should not be set directly by
     * user code.
     *
     * @c Cursor is not typically used standalone. @c TextInput composes
     * a @c Typer and a @c Cursor together into a complete input field.
     *
     * ### Internal event registration
     * @c registerEvents() wires the blink animation to the framework's
     * per-frame @c "update" event, so the cursor blinks automatically
     * once registered with a manager.
     *
     * @see TextInput, Typer, Text
     */
    class Cursor : public ml::Text
    {
        inline static sf::Clock clock;

        using Text::setString; ///< Hidden — cursor string is managed internally.

        void registerEvents();

    public:
        using ml::Text::Text;

        /**
         * @brief Construct a @c Cursor with a given font.
         *
         * Sets up the cursor character and registers the blink animation
         * via @c registerEvents().
         *
         * @param font Font used to render the cursor glyph. Defaults to
         *             the built-in Arial font via @c FontManager<>::getDefault().
         */
        Cursor(const sf::Font& font = FontManager<>::getDefault());
    };

} // namespace ml

#endif // CURSOR_H
