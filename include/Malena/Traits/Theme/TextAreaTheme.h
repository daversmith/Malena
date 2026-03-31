//
// TextAreaTheme.h
//

#ifndef TEXTAREATHEME_H
#define TEXTAREATHEME_H

#pragma once

#include <Malena/Traits/Theme/TextInputTheme.h>

namespace ml
{
    /**
     * @brief Color tokens for @c TextArea.
     * @ingroup Traits
     *
     * Extends @c TextInputTheme with scrollbar colors.
     * Note: scrollbar width is a geometry value and lives in @c TextInputSettings.
     *
     * @see TextInputTheme, TextInputSettings, TextAreaStyle, TextArea
     */
    struct TextAreaTheme : TextInputTheme
    {
        sf::Color scrollBarColor      = sf::Color(100, 100, 100);
        sf::Color scrollBarTrackColor = sf::Color(50,  50,  50);
        float     scrollBarWidth      = 8.f;

        // ── applyFrom ─────────────────────────────────────────────────────────

        TextAreaTheme& applyFrom(const Theme& t) override
        {
            TextInputTheme::applyFrom(t);
            scrollBarColor      = t.border;
            scrollBarTrackColor = t.surface;
            return *this;
        }

        // ── Assignment from parent levels ─────────────────────────────────────

        TextAreaTheme& operator=(const TextInputTheme& ti)
        { static_cast<TextInputTheme&>(*this) = ti; return *this; }

        TextAreaTheme& operator=(const InputTheme& i)
        { static_cast<TextInputTheme&>(*this) = i; return *this; }

        TextAreaTheme& operator=(const ControlTheme& c)
        { static_cast<TextInputTheme&>(*this) = c; return *this; }

        TextAreaTheme& operator=(const GraphicTheme& g)
        { static_cast<TextInputTheme&>(*this) = g; return *this; }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Color getScrollBarColor()      const { return scrollBarColor; }
        [[nodiscard]] sf::Color getScrollBarTrackColor() const { return scrollBarTrackColor; }
        [[nodiscard]] float     getScrollBarWidth()      const { return scrollBarWidth; }

        void setScrollBarColor(const sf::Color& c)      { scrollBarColor      = c; }
        void setScrollBarTrackColor(const sf::Color& c) { scrollBarTrackColor = c; }
        void setScrollBarWidth(float w)                 { scrollBarWidth      = w; }
    };

} // namespace ml

#endif // TEXTAREATHEME_H
