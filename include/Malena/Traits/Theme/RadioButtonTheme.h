//
// RadioButtonTheme.h
//

#ifndef MALENA_RADIOBUTTONTHEME_H
#define MALENA_RADIOBUTTONTHEME_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Theme/ControlTheme.h>

namespace ml
{
    /**
     * @brief Color and font tokens for @c RadioButton.
     * @ingroup Traits
     *
     * Geometry lives in @c RadioButtonSettings.
     *
     * @see ControlTheme, RadioButtonSettings, RadioButtonStyle, RadioButton
     */
    struct RadioButtonTheme : ControlTheme
    {
        sf::Color ringColor          = sf::Color(120, 120, 120);
        sf::Color ringHoverColor     = sf::Color(80,  80,  80);
        sf::Color ringSelectedColor  = sf::Color(70,  130, 230);
        sf::Color dotColor           = sf::Color(70,  130, 230);
        sf::Color ringDisabledColor  = sf::Color(180, 180, 180);
        sf::Color dotDisabledColor   = sf::Color(200, 200, 200);
        sf::Color labelColor         = sf::Color::White;
        sf::Color labelDisabledColor = sf::Color(160, 160, 160);
        float     ringThickness      = 2.f;

        // ── applyFrom ─────────────────────────────────────────────────────────

        RadioButtonTheme& applyFrom(const Theme& t) override
        {
            ControlTheme::applyFrom(t);
            ringColor          = t.border;
            ringHoverColor     = t.secondary;
            ringSelectedColor  = t.primary;
            dotColor           = t.primary;
            ringDisabledColor  = t.disabled;
            dotDisabledColor   = t.disabled;
            labelColor         = t.onSurface;
            labelDisabledColor = t.onDisabled;
            return *this;
        }

        // ── Assignment from parent levels ─────────────────────────────────────

        RadioButtonTheme& operator=(const ControlTheme& c)
        { static_cast<ControlTheme&>(*this) = c; return *this; }

        RadioButtonTheme& operator=(const GraphicTheme& g)
        { static_cast<ControlTheme&>(*this) = g; return *this; }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Color getRingColor()          const { return ringColor; }
        [[nodiscard]] sf::Color getRingHoverColor()     const { return ringHoverColor; }
        [[nodiscard]] sf::Color getRingSelectedColor()  const { return ringSelectedColor; }
        [[nodiscard]] sf::Color getDotColor()           const { return dotColor; }
        [[nodiscard]] sf::Color getRingDisabledColor()  const { return ringDisabledColor; }
        [[nodiscard]] sf::Color getDotDisabledColor()   const { return dotDisabledColor; }
        [[nodiscard]] sf::Color getLabelColor()         const { return labelColor; }
        [[nodiscard]] sf::Color getLabelDisabledColor() const { return labelDisabledColor; }
        [[nodiscard]] float     getRingThickness()      const { return ringThickness; }

        void setRingColor(const sf::Color& c)          { ringColor          = c; }
        void setRingHoverColor(const sf::Color& c)     { ringHoverColor     = c; }
        void setRingSelectedColor(const sf::Color& c)  { ringSelectedColor  = c; }
        void setDotColor(const sf::Color& c)           { dotColor           = c; }
        void setRingDisabledColor(const sf::Color& c)  { ringDisabledColor  = c; }
        void setDotDisabledColor(const sf::Color& c)   { dotDisabledColor   = c; }
        void setLabelColor(const sf::Color& c)         { labelColor         = c; }
        void setLabelDisabledColor(const sf::Color& c) { labelDisabledColor = c; }
        void setRingThickness(float t)                 { ringThickness      = t; }
    };

} // namespace ml

#endif // MALENA_RADIOBUTTONTHEME_H
