//
// ListItemTheme.h
//

#ifndef LISTITEMTHEME_H
#define LISTITEMTHEME_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Theme/ControlTheme.h>

namespace ml
{
    /**
     * @brief Color and font tokens for @c ListItem.
     * @ingroup Traits
     *
     * Geometry lives in @c ListItemSettings. @c textColor (label),
     * @c mutedColor (description), @c hoverColor, @c disabledColor,
     * @c disabledTextColor, @c font, @c fontSize and @c padding are
     * inherited from @c ControlTheme.
     *
     * @see ControlTheme, ListItemSettings, ListItemStyle, ListItem
     */
    struct ListItemTheme : ControlTheme
    {
        sf::Color bgIdle          = sf::Color::Transparent;
        sf::Color bgHover         = sf::Color(255, 255, 255, 18);
        sf::Color bgDisabled      = sf::Color::Transparent;
        sf::Color dividerColor    = sf::Color(80,  80,  80);
        float     dividerThickness = 1.f;

        // ── applyFrom ─────────────────────────────────────────────────────────

        ListItemTheme& applyFrom(const Theme& t) override
        {
            ControlTheme::applyFrom(t);
            bgIdle          = sf::Color::Transparent;
            bgHover         = sf::Color(t.onSurface.r,
                                        t.onSurface.g,
                                        t.onSurface.b, 18);
            bgDisabled      = sf::Color::Transparent;
            dividerColor    = t.border;
            // List items use tighter vertical padding than the global spacing
            padding         = 6.f;
            return *this;
        }

        // ── Assignment from parent levels ─────────────────────────────────────

        ListItemTheme& operator=(const ControlTheme& c)
        { static_cast<ControlTheme&>(*this) = c; return *this; }

        ListItemTheme& operator=(const GraphicTheme& g)
        { static_cast<ControlTheme&>(*this) = g; return *this; }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Color getBgIdle()           const { return bgIdle; }
        [[nodiscard]] sf::Color getBgHover()          const { return bgHover; }
        [[nodiscard]] sf::Color getBgDisabled()       const { return bgDisabled; }
        [[nodiscard]] sf::Color getDividerColor()     const { return dividerColor; }
        [[nodiscard]] float     getDividerThickness() const { return dividerThickness; }

        void setBgIdle(const sf::Color& c)        { bgIdle           = c; }
        void setBgHover(const sf::Color& c)       { bgHover          = c; }
        void setBgDisabled(const sf::Color& c)    { bgDisabled       = c; }
        void setDividerColor(const sf::Color& c)  { dividerColor     = c; }
        void setDividerThickness(float t)         { dividerThickness = t; }
    };

} // namespace ml

#endif // LISTITEMTHEME_H
