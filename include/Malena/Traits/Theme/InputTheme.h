//
// InputTheme.h
//

#ifndef MALENA_INPUTTHEME_H
#define MALENA_INPUTTHEME_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Theme/ControlTheme.h>

namespace ml
{
    /**
     * @brief Color and font tokens shared by all input controls.
     * @ingroup Traits
     *
     * Base for @c TextInputTheme and @c SelectTheme.
     * Geometry lives in @c InputSettings.
     *
     * @see ControlTheme, InputSettings, TextInputTheme, SelectTheme
     */
    struct InputTheme : ControlTheme
    {
        sf::Color bgIdle           = sf::Color(40,  40,  40);
        sf::Color bgFocused        = sf::Color(50,  50,  50);
        sf::Color bgDisabled       = sf::Color(30,  30,  30);
        sf::Color borderIdle       = sf::Color(100, 100, 100);
        sf::Color borderFocused    = sf::Color(70,  130, 230);
        sf::Color borderError      = sf::Color(220, 70,  70);
        sf::Color borderDisabled   = sf::Color(60,  60,  60);
        sf::Color placeholderColor = sf::Color(120, 120, 120);
        float     borderThickness  = 1.5f;

        // ── applyFrom ─────────────────────────────────────────────────────────

        InputTheme& applyFrom(const Theme& t) override
        {
            ControlTheme::applyFrom(t);
            bgIdle           = t.surface;
            bgFocused        = sf::Color(
                std::min(255, t.surface.r + 10),
                std::min(255, t.surface.g + 10),
                std::min(255, t.surface.b + 10));
            bgDisabled       = t.disabled;
            borderIdle       = t.border;
            borderFocused    = t.borderFocus;
            borderError      = t.error;
            borderDisabled   = t.disabled;
            placeholderColor = t.muted;
            borderThickness  = t.borderThickness;
            return *this;
        }

        // ── Assignment from parent levels ─────────────────────────────────────

        InputTheme& operator=(const ControlTheme& c)
        { static_cast<ControlTheme&>(*this) = c; return *this; }

        InputTheme& operator=(const GraphicTheme& g)
        { static_cast<ControlTheme&>(*this) = g; return *this; }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Color getBgIdle()           const { return bgIdle; }
        [[nodiscard]] sf::Color getBgFocused()        const { return bgFocused; }
        [[nodiscard]] sf::Color getBgDisabled()       const { return bgDisabled; }
        [[nodiscard]] sf::Color getBorderIdle()       const { return borderIdle; }
        [[nodiscard]] sf::Color getBorderFocused()    const { return borderFocused; }
        [[nodiscard]] sf::Color getBorderError()      const { return borderError; }
        [[nodiscard]] sf::Color getBorderDisabled()   const { return borderDisabled; }
        [[nodiscard]] sf::Color getPlaceholderColor() const { return placeholderColor; }
        [[nodiscard]] float     getBorderThickness()  const { return borderThickness; }

        void setBgIdle(const sf::Color& c)           { bgIdle           = c; }
        void setBgFocused(const sf::Color& c)        { bgFocused        = c; }
        void setBgDisabled(const sf::Color& c)       { bgDisabled       = c; }
        void setBorderIdle(const sf::Color& c)       { borderIdle       = c; }
        void setBorderFocused(const sf::Color& c)    { borderFocused    = c; }
        void setBorderError(const sf::Color& c)      { borderError      = c; }
        void setBorderDisabled(const sf::Color& c)   { borderDisabled   = c; }
        void setPlaceholderColor(const sf::Color& c) { placeholderColor = c; }
        void setBorderThickness(float t)             { borderThickness  = t; }
    };

} // namespace ml

#endif // MALENA_INPUTTHEME_H
