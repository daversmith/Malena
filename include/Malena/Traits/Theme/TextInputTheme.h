//
// TextInputTheme.h
//

#ifndef MALENA_TEXTINPUTTHEME_H
#define MALENA_TEXTINPUTTHEME_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Theme/InputTheme.h>

namespace ml
{
    /**
     * @brief Color tokens for @c TextInput and @c TextArea.
     * @ingroup Traits
     *
     * Extends @c InputTheme with selection highlight and cursor colors.
     *
     * @see InputTheme, TextInputSettings, TextInputStyle, TextInput
     */
    struct TextInputTheme : InputTheme
    {
        sf::Color selectionColor = sf::Color(70, 130, 230, 120);
        sf::Color cursorColor    = sf::Color::White;

        // ── applyFrom ─────────────────────────────────────────────────────────

        TextInputTheme& applyFrom(const Theme& t) override
        {
            InputTheme::applyFrom(t);
            selectionColor = sf::Color(t.primary.r, t.primary.g, t.primary.b, 120);
            cursorColor    = t.onSurface;
            return *this;
        }

        // ── Assignment from parent levels ─────────────────────────────────────

        TextInputTheme& operator=(const InputTheme& i)
        { static_cast<InputTheme&>(*this) = i; return *this; }

        TextInputTheme& operator=(const ControlTheme& c)
        { static_cast<InputTheme&>(*this) = c; return *this; }

        TextInputTheme& operator=(const GraphicTheme& g)
        { static_cast<InputTheme&>(*this) = g; return *this; }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Color getSelectionColor() const { return selectionColor; }
        [[nodiscard]] sf::Color getCursorColor()    const { return cursorColor; }

        void setSelectionColor(const sf::Color& c) { selectionColor = c; }
        void setCursorColor(const sf::Color& c)    { cursorColor    = c; }
    };

} // namespace ml

#endif // MALENA_TEXTINPUTTHEME_H
