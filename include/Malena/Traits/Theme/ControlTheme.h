//
// ControlTheme.h
//

#ifndef CONTROLTHEME_H
#define CONTROLTHEME_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Theme/GraphicTheme.h>
#include <Malena/Resources/FontManager.h>

namespace ml
{
    // ── ControlTheme ──────────────────────────────────────────────────────────

    /**
     * @brief Theme tokens for interactive UI controls.
     * @ingroup Settings
     *
     * Extends @c GraphicTheme with tokens for hover, focus, disabled, and
     * text states. All controls — toggles, buttons, checkboxes, etc. —
     * use @c ControlTheme or a subclass.
     *
     * @see GraphicTheme, ToggleTheme
     */
    struct ControlTheme : GraphicTheme
    {
        sf::Color hoverColor        = sf::Color(70,  130, 230);
        sf::Color focusColor        = sf::Color(100, 60,  200);
        sf::Color errorColor        = sf::Color(220, 70,  70);
        sf::Color disabledColor     = sf::Color(60,  60,  60);
        sf::Color disabledTextColor = sf::Color(120, 120, 120);
        sf::Color textColor         = sf::Color::White;
        sf::Color mutedColor        = sf::Color(120, 120, 120);
        float     padding           = 8.f;

        const sf::Font* font        = &FontManager<>::getDefault();
        unsigned int    fontSize    = 14;
        unsigned int    fontSizeSmall = 11;

        // ── applyFrom ─────────────────────────────────────────────────────────

    	ControlTheme& applyFrom(const Theme& t) override
    	{
    		GraphicTheme::applyFrom(t);
    		hoverColor        = t.secondary;
    		focusColor        = t.borderFocus;
    		errorColor        = t.error;
    		disabledColor     = t.disabled;
    		disabledTextColor = t.onDisabled;
    		textColor         = t.onSurface;
    		mutedColor        = t.muted;
    		padding           = t.spacing;
    		font              = t.font ? t.font : &FontManager<>::getDefault();  // ← guard
    		fontSize          = t.fontSize;
    		fontSizeSmall     = t.fontSizeSmall;
    		return *this;
    	}

        // ── Assignment from parent ────────────────────────────────────────────

        ControlTheme& operator=(const GraphicTheme& g)
        {
            static_cast<GraphicTheme&>(*this) = g;
            return *this;
        }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Color      getHoverColor()        const { return hoverColor; }
        [[nodiscard]] sf::Color      getFocusColor()        const { return focusColor; }
        [[nodiscard]] sf::Color      getErrorColor()        const { return errorColor; }
        [[nodiscard]] sf::Color      getDisabledColor()     const { return disabledColor; }
        [[nodiscard]] sf::Color      getDisabledTextColor() const { return disabledTextColor; }
        [[nodiscard]] sf::Color      getTextColor()         const { return textColor; }
        [[nodiscard]] sf::Color      getMutedColor()        const { return mutedColor; }
        [[nodiscard]] float          getPadding()           const { return padding; }
        [[nodiscard]] const sf::Font* getFont()             const { return font; }
        [[nodiscard]] unsigned int   getFontSize()          const { return fontSize; }
        [[nodiscard]] unsigned int   getFontSizeSmall()     const { return fontSizeSmall; }

        void setHoverColor(const sf::Color& c)        { hoverColor        = c; }
        void setFocusColor(const sf::Color& c)        { focusColor        = c; }
        void setErrorColor(const sf::Color& c)        { errorColor        = c; }
        void setDisabledColor(const sf::Color& c)     { disabledColor     = c; }
        void setDisabledTextColor(const sf::Color& c) { disabledTextColor = c; }
        void setTextColor(const sf::Color& c)         { textColor         = c; }
        void setMutedColor(const sf::Color& c)        { mutedColor        = c; }
        void setPadding(float p)                      { padding           = p; }
        void setFont(const sf::Font& f)               { font              = &f; }
        void setFont(const sf::Font&&) = delete;
        void setFontSize(unsigned int s)              { fontSize          = s; }
        void setFontSizeSmall(unsigned int s)         { fontSizeSmall     = s; }
    };

} // namespace ml

#endif // CONTROLTHEME_H
