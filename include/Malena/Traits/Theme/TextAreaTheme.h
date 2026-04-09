//
// TextAreaTheme.h
//

#ifndef TEXTAREATHEME_H
#define TEXTAREATHEME_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Manifests/Theme.h>
#include <SFML/Graphics/Color.hpp>

namespace ml
{
    /**
     * @brief Scrollbar color tokens for @c TextArea.
     * @ingroup Traits
     *
     * Intentionally does NOT inherit @c TextInputTheme — @c TextArea already
     * receives those fields through @c TextInput. Keeping this struct standalone
     * avoids the diamond inheritance that would otherwise occur:
     *
     *   TextArea → TextInput    → TextInputTheme → InputTheme → ControlTheme
     *   TextArea → TextAreaTheme → (standalone — no shared base)
     *
     * @see TextInputTheme, TextInputSettings, TextAreaStyle, TextArea
     */
    struct TextAreaTheme
    {
        sf::Color scrollBarColor      = sf::Color(100, 100, 100);
        sf::Color scrollBarTrackColor = sf::Color(50,  50,  50);
        float     scrollBarWidth      = 8.f;

        // ── applyFrom ─────────────────────────────────────────────────────────

        TextAreaTheme& applyFrom(const Theme& t)
        {
            scrollBarColor      = t.border;
            scrollBarTrackColor = t.surface;
            return *this;
        }

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
