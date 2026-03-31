//
// RadioGroupTheme.h
//

#ifndef RADIOGROUPTHEME_H
#define RADIOGROUPTHEME_H

#pragma once

#include <Malena/Traits/Theme/RadioButtonTheme.h>

namespace ml
{
    /**
     * @brief Color tokens for @c RadioGroup.
     * @ingroup Traits
     *
     * Extends @c RadioButtonTheme with colors for the optional background panel.
     *
     * @see RadioButtonTheme, RadioGroupSettings, RadioGroupStyle, RadioGroup
     */
    struct RadioGroupTheme : RadioButtonTheme
    {
        sf::Color bgColor        = sf::Color::Transparent;
        sf::Color bgOutlineColor = sf::Color::Transparent;

        // ── applyFrom ─────────────────────────────────────────────────────────

        RadioGroupTheme& applyFrom(const Theme& t) override
        {
            RadioButtonTheme::applyFrom(t);
            bgColor        = sf::Color(t.surface.r, t.surface.g, t.surface.b, 220);
            bgOutlineColor = t.border;
            return *this;
        }

        // ── Assignment from parent levels ─────────────────────────────────────

        RadioGroupTheme& operator=(const RadioButtonTheme& r)
        { static_cast<RadioButtonTheme&>(*this) = r; return *this; }

        RadioGroupTheme& operator=(const ControlTheme& c)
        { static_cast<RadioButtonTheme&>(*this) = c; return *this; }

        RadioGroupTheme& operator=(const GraphicTheme& g)
        { static_cast<RadioButtonTheme&>(*this) = g; return *this; }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Color getBgColor()        const { return bgColor; }
        [[nodiscard]] sf::Color getBgOutlineColor() const { return bgOutlineColor; }

        void setBgColor(const sf::Color& c)        { bgColor        = c; }
        void setBgOutlineColor(const sf::Color& c) { bgOutlineColor = c; }
    };

} // namespace ml

#endif // RADIOGROUPTHEME_H
