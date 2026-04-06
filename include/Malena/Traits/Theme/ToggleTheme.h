//
// ToggleTheme.h
//

#ifndef TOGGLETHEME_H
#define TOGGLETHEME_H

#pragma once

#include <Malena/Traits/Theme/ControlTheme.h>

namespace ml
{
    // ── ToggleTheme ───────────────────────────────────────────────────────────

    /**
     * @brief Theme tokens shared by all toggle variants.
     * @ingroup Settings
     *
     * Extends @c ControlTheme with the active/inactive color pair used by
     * every toggle style — the "on" color, the "off" color, and the text
     * color on top of the active state.
     *
     * Style-specific theme structs (@c PillTheme, @c SegmentTheme,
     * @c ButtonTheme) inherit from this.
     *
     * @see ControlTheme, PillTheme, SegmentTheme, ButtonTheme
     */
    struct ToggleTheme : ControlTheme
    {
        sf::Color activeColor     = sf::Color(100, 60,  200);
        sf::Color inactiveColor   = sf::Color(40,  40,  40);
        sf::Color activeTextColor = sf::Color::White;

        // ── applyFrom ─────────────────────────────────────────────────────────

        ToggleTheme& applyFrom(const Theme& t) override
        {
            ControlTheme::applyFrom(t);
            activeColor     = t.primary;
            inactiveColor   = t.surface;
            activeTextColor = t.onPrimary;
            return *this;
        }

        // ── Assignment from parent ────────────────────────────────────────────

        ToggleTheme& operator=(const ControlTheme& c)
        {
            static_cast<ControlTheme&>(*this) = c;
            return *this;
        }

        ToggleTheme& operator=(const GraphicTheme& g)
        {
            static_cast<ControlTheme&>(*this) = g;
            return *this;
        }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Color getActiveColor()     const { return activeColor; }
        [[nodiscard]] sf::Color getInactiveColor()   const { return inactiveColor; }
        [[nodiscard]] sf::Color getActiveTextColor() const { return activeTextColor; }

        void setActiveColor(const sf::Color& c)     { activeColor     = c; }
        void setInactiveColor(const sf::Color& c)   { inactiveColor   = c; }
        void setActiveTextColor(const sf::Color& c) { activeTextColor = c; }
    };

} // namespace ml

#endif // TOGGLETHEME_H
