//
// ButtonTheme.h
//

#ifndef BUTTONTHEME_H
#define BUTTONTHEME_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Theme/ToggleTheme.h>

namespace ml
{
    // ── ButtonTheme ───────────────────────────────────────────────────────────

    /**
     * @brief Theme tokens for the @c ButtonToggle (rectangular toggle button).
     * @ingroup Settings
     *
     * Extends @c ToggleTheme with color tokens for the button style — the
     * on/off background colors, their corresponding text colors, and the
     * border color.
     *
     * @see ToggleTheme, ButtonSettings, ButtonStyle, ButtonToggle
     */
    struct ButtonTheme : ToggleTheme
    {
        sf::Color onColor         = sf::Color(100, 60,  200);
        sf::Color offColor        = sf::Color(40,  40,  40);
        sf::Color onTextColor     = sf::Color::White;
        sf::Color offTextColor    = sf::Color(180, 180, 180);
        sf::Color borderColor     = sf::Color(100, 100, 100);
        float     borderThickness = 1.5f;
        float     buttonRadius    = 8.f;

        // ── applyFrom ─────────────────────────────────────────────────────────

        ButtonTheme& applyFrom(const Theme& t) override
        {
            ToggleTheme::applyFrom(t);
            onColor         = t.primary;
            offColor        = t.surface;
            onTextColor     = t.onPrimary;
            offTextColor    = t.onSurface;
            borderColor     = t.border;
            borderThickness = t.borderThickness;
            buttonRadius    = t.radius;
            return *this;
        }

        // ── Assignment from parent levels ─────────────────────────────────────

        ButtonTheme& operator=(const ToggleTheme& t)
        {
            static_cast<ToggleTheme&>(*this) = t;
            return *this;
        }

        ButtonTheme& operator=(const ControlTheme& c)
        {
            static_cast<ToggleTheme&>(*this) = c;
            return *this;
        }

        ButtonTheme& operator=(const GraphicTheme& g)
        {
            static_cast<ToggleTheme&>(*this) = g;
            return *this;
        }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Color getOnColor()         const { return onColor; }
        [[nodiscard]] sf::Color getOffColor()        const { return offColor; }
        [[nodiscard]] sf::Color getOnTextColor()     const { return onTextColor; }
        [[nodiscard]] sf::Color getOffTextColor()    const { return offTextColor; }
        [[nodiscard]] sf::Color getBorderColor()     const { return borderColor; }
        [[nodiscard]] float     getBorderThickness() const { return borderThickness; }
        [[nodiscard]] float     getButtonRadius()    const { return buttonRadius; }

        void setOnColor(const sf::Color& c)         { onColor         = c; }
        void setOffColor(const sf::Color& c)        { offColor        = c; }
        void setOnTextColor(const sf::Color& c)     { onTextColor     = c; }
        void setOffTextColor(const sf::Color& c)    { offTextColor    = c; }
        void setBorderColor(const sf::Color& c)     { borderColor     = c; }
        void setBorderThickness(float t)            { borderThickness = t; }
        void setButtonRadius(float r)               { buttonRadius    = r; }
    };

} // namespace ml

#endif // BUTTONTHEME_H