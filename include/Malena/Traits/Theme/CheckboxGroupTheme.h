//
// CheckboxGroupTheme.h
//

#ifndef CHECKBOXGROUPTHEME_H
#define CHECKBOXGROUPTHEME_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Theme/CheckboxTheme.h>

namespace ml
{
    /**
     * @brief Color tokens for @c CheckboxGroup.
     * @ingroup Traits
     *
     * Extends @c CheckboxTheme with colors for the optional background panel.
     * The inherited @c CheckboxTheme fields serve as defaults for every
     * item the group creates.
     *
     * @see CheckboxTheme, CheckboxGroupSettings, CheckboxGroupStyle, CheckboxGroup
     */
    struct CheckboxGroupTheme : CheckboxTheme
    {
        sf::Color bgColor        = sf::Color::Transparent;
        sf::Color bgOutlineColor = sf::Color::Transparent;

        // ── applyFrom ─────────────────────────────────────────────────────────

        CheckboxGroupTheme& applyFrom(const Theme& t) override
        {
            CheckboxTheme::applyFrom(t);
            bgColor        = sf::Color(t.surface.r, t.surface.g, t.surface.b, 220);
            bgOutlineColor = t.border;
            return *this;
        }

        // ── Assignment from parent levels ─────────────────────────────────────

        CheckboxGroupTheme& operator=(const CheckboxTheme& c)
        { static_cast<CheckboxTheme&>(*this) = c; return *this; }

        CheckboxGroupTheme& operator=(const ControlTheme& c)
        { static_cast<CheckboxTheme&>(*this) = c; return *this; }

        CheckboxGroupTheme& operator=(const GraphicTheme& g)
        { static_cast<CheckboxTheme&>(*this) = g; return *this; }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Color getBgColor()        const { return bgColor; }
        [[nodiscard]] sf::Color getBgOutlineColor() const { return bgOutlineColor; }

        void setBgColor(const sf::Color& c)        { bgColor        = c; }
        void setBgOutlineColor(const sf::Color& c) { bgOutlineColor = c; }
    };

} // namespace ml

#endif // CHECKBOXGROUPTHEME_H
