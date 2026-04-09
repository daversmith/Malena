//
// PillTheme.h
//

#ifndef PILLTHEME_H
#define PILLTHEME_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Theme/ToggleTheme.h>

namespace ml
{
    // ── PillTheme ─────────────────────────────────────────────────────────────

    /**
     * @brief Theme tokens for the @c PillToggle (iOS-style oval switch).
     * @ingroup Settings
     *
     * Extends @c ToggleTheme with all color tokens specific to the pill
     * switch — track colors, thumb color, and inside label colors.
     *
     * Size and animation speed live in @c PillSettings, not here, because
     * they are layout/behaviour values rather than theme-derived tokens.
     *
     * ### Usage — apply from global theme
     * @code
     * PillTheme pt;
     * pt.applyFrom(ml::ThemeManager::get());
     * @endcode
     *
     * ### Usage — assign from a broader theme struct
     * @code
     * PillTheme pt;
     * pt = myToggleTheme;   // copies ToggleTheme fields, leaves PillTheme fields
     * @endcode
     *
     * ### Usage — set individual tokens
     * @code
     * pill.setTrackOnColor(sf::Color(70, 200, 100));
     * pill.setThumbColor(sf::Color::White);
     * @endcode
     *
     * @see ToggleTheme, PillSettings, PillStyle, PillToggle
     */
    struct PillTheme : ToggleTheme
    {
        sf::Color trackOnColor           = sf::Color(100, 60,  200);
        sf::Color trackOffColor          = sf::Color(120, 120, 120);
        sf::Color thumbColor             = sf::Color::White;
        sf::Color thumbDisabledColor     = sf::Color(220, 220, 220);
        sf::Color pillLabelActiveColor   = sf::Color::White;
        sf::Color pillLabelInactiveColor = sf::Color(200, 200, 200, 180);

        // ── applyFrom ─────────────────────────────────────────────────────────

        PillTheme& applyFrom(const Theme& t) override
        {
            ToggleTheme::applyFrom(t);
            trackOnColor           = t.primary;
            trackOffColor          = t.border;
            thumbColor             = t.onPrimary;
            thumbDisabledColor     = t.onDisabled;
            pillLabelActiveColor   = t.onPrimary;
            pillLabelInactiveColor = sf::Color(
                t.onSurface.r, t.onSurface.g, t.onSurface.b, 180);
            return *this;
        }

        // ── Assignment from parent levels ─────────────────────────────────────

        PillTheme& operator=(const ToggleTheme& t)
        {
            static_cast<ToggleTheme&>(*this) = t;
            return *this;
        }

        PillTheme& operator=(const ControlTheme& c)
        {
            static_cast<ToggleTheme&>(*this) = c;
            return *this;
        }

        PillTheme& operator=(const GraphicTheme& g)
        {
            static_cast<ToggleTheme&>(*this) = g;
            return *this;
        }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Color getTrackOnColor()           const { return trackOnColor; }
        [[nodiscard]] sf::Color getTrackOffColor()          const { return trackOffColor; }
        [[nodiscard]] sf::Color getThumbColor()             const { return thumbColor; }
        [[nodiscard]] sf::Color getThumbDisabledColor()     const { return thumbDisabledColor; }
        [[nodiscard]] sf::Color getPillLabelActiveColor()   const { return pillLabelActiveColor; }
        [[nodiscard]] sf::Color getPillLabelInactiveColor() const { return pillLabelInactiveColor; }

        void setTrackOnColor(const sf::Color& c)           { trackOnColor           = c; }
        void setTrackOffColor(const sf::Color& c)          { trackOffColor          = c; }
        void setThumbColor(const sf::Color& c)             { thumbColor             = c; }
        void setThumbDisabledColor(const sf::Color& c)     { thumbDisabledColor     = c; }
        void setPillLabelActiveColor(const sf::Color& c)   { pillLabelActiveColor   = c; }
        void setPillLabelInactiveColor(const sf::Color& c) { pillLabelInactiveColor = c; }
    };

} // namespace ml

#endif // PILLTHEME_H
