//
// ListTheme.h
//

#ifndef LISTTHEME_H
#define LISTTHEME_H

#pragma once

#include <Malena/Traits/Theme/ControlTheme.h>

namespace ml
{
    /**
     * @brief Color tokens for @c List.
     * @ingroup Traits
     *
     * Geometry lives in @c ListSettings.
     *
     * @see ControlTheme, ListSettings, ListStyle, List
     */
    struct ListTheme : ControlTheme
    {
        sf::Color bgColor            = sf::Color(30, 30, 30);
        sf::Color dividerColor       = sf::Color(80, 80, 80);

        // ── applyFrom ─────────────────────────────────────────────────────────

        ListTheme& applyFrom(const Theme& t) override
        {
            ControlTheme::applyFrom(t);
            bgColor      = t.surface;
            dividerColor = t.border;
            return *this;
        }

        // ── Assignment from parent levels ─────────────────────────────────────

        ListTheme& operator=(const ControlTheme& c)
        { static_cast<ControlTheme&>(*this) = c; return *this; }

        ListTheme& operator=(const GraphicTheme& g)
        { static_cast<ControlTheme&>(*this) = g; return *this; }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Color getBgColor()      const { return bgColor; }
        [[nodiscard]] sf::Color getDividerColor() const { return dividerColor; }

        void setBgColor(const sf::Color& c)      { bgColor      = c; }
        void setDividerColor(const sf::Color& c) { dividerColor = c; }
    };

} // namespace ml

#endif // LISTTHEME_H
