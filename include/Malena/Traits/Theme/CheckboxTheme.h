//
// CheckboxTheme.h
//

#ifndef CHECKBOXTHEME_H
#define CHECKBOXTHEME_H

#pragma once

#include <Malena/Traits/Theme/ControlTheme.h>

namespace ml
{
    /**
     * @brief Color and font tokens for @c Checkbox.
     * @ingroup Traits
     *
     * Geometry lives in @c CheckboxSettings.
     *
     * @see ControlTheme, CheckboxSettings, CheckboxStyle, Checkbox
     */
    struct CheckboxTheme : ControlTheme
    {
        sf::Color boxColor           = sf::Color(60,  60,  60);
        sf::Color boxHoverColor      = sf::Color(80,  80,  80);
        sf::Color boxCheckedColor    = sf::Color(70,  130, 230);
        sf::Color boxDisabledColor   = sf::Color(180, 180, 180);
        sf::Color checkColor         = sf::Color::White;
        sf::Color checkDisabledColor = sf::Color(220, 220, 220);
        sf::Color outlineColor       = sf::Color(120, 120, 120);
        sf::Color labelColor         = sf::Color::White;
        sf::Color labelDisabledColor = sf::Color(160, 160, 160);

        // ── applyFrom ─────────────────────────────────────────────────────────

        CheckboxTheme& applyFrom(const Theme& t) override
        {
            ControlTheme::applyFrom(t);
            boxColor           = t.surface;
            boxHoverColor      = t.secondary;
            boxCheckedColor    = t.primary;
            boxDisabledColor   = t.disabled;
            checkColor         = t.onPrimary;
            checkDisabledColor = t.onDisabled;
            outlineColor       = t.border;
            labelColor         = t.onSurface;
            labelDisabledColor = t.onDisabled;
            return *this;
        }

        // ── Assignment from parent levels ─────────────────────────────────────

        CheckboxTheme& operator=(const ControlTheme& c)
        { static_cast<ControlTheme&>(*this) = c; return *this; }

        CheckboxTheme& operator=(const GraphicTheme& g)
        { static_cast<ControlTheme&>(*this) = g; return *this; }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Color getBoxColor()           const { return boxColor; }
        [[nodiscard]] sf::Color getBoxHoverColor()      const { return boxHoverColor; }
        [[nodiscard]] sf::Color getBoxCheckedColor()    const { return boxCheckedColor; }
        [[nodiscard]] sf::Color getBoxDisabledColor()   const { return boxDisabledColor; }
        [[nodiscard]] sf::Color getCheckColor()         const { return checkColor; }
        [[nodiscard]] sf::Color getCheckDisabledColor() const { return checkDisabledColor; }
        [[nodiscard]] sf::Color getOutlineColor()       const { return outlineColor; }
        [[nodiscard]] sf::Color getLabelColor()         const { return labelColor; }
        [[nodiscard]] sf::Color getLabelDisabledColor() const { return labelDisabledColor; }

        void setBoxColor(const sf::Color& c)           { boxColor           = c; }
        void setBoxHoverColor(const sf::Color& c)      { boxHoverColor      = c; }
        void setBoxCheckedColor(const sf::Color& c)    { boxCheckedColor    = c; }
        void setBoxDisabledColor(const sf::Color& c)   { boxDisabledColor   = c; }
        void setCheckColor(const sf::Color& c)         { checkColor         = c; }
        void setCheckDisabledColor(const sf::Color& c) { checkDisabledColor = c; }
        void setOutlineColor(const sf::Color& c)       { outlineColor       = c; }
        void setLabelColor(const sf::Color& c)         { labelColor         = c; }
        void setLabelDisabledColor(const sf::Color& c) { labelDisabledColor = c; }
    };

} // namespace ml

#endif // CHECKBOXTHEME_H
