//
// ControlSettings.h
//

#ifndef CONTROLSETTINGS_H
#define CONTROLSETTINGS_H

#pragma once

#include <Malena/Settings/GraphicSettings.h>

namespace ml
{
    // ── ControlSettings ───────────────────────────────────────────────────────

    /**
     * @brief Settings for interactive UI controls.
     * @ingroup Settings
     *
     * Extends @c GraphicSettings with tokens relevant to clickable, hoverable,
     * and focusable components — hover color, focus color, disabled state,
     * and internal padding.
     *
     * All controls (@c Toggle, @c Select, @c Checkbox, @c RadioButton, etc.)
     * use @c ControlSettings or a subclass as their settings base.
     *
     * @see GraphicSettings, ToggleSettings, ThemeManager
     */
    struct ControlSettings : GraphicSettings
    {
        /** Fill color when the mouse hovers over the component. */
        sf::Color hoverColor    = ThemeManager::get().secondary;

        /** Outline color when the component has keyboard focus. */
        sf::Color focusColor    = ThemeManager::get().borderFocus;

        /** Fill color when the component is in an error state. */
        sf::Color errorColor    = ThemeManager::get().error;

        /** Fill color when the component is disabled. */
        sf::Color disabledColor = ThemeManager::get().disabled;

        /** Text/icon color when the component is disabled. */
        sf::Color disabledTextColor = ThemeManager::get().onDisabled;

        /** Default text/label color. */
        sf::Color textColor     = ThemeManager::get().onSurface;

        /** Muted text color — placeholders, descriptions. */
        sf::Color mutedColor    = ThemeManager::get().muted;

        /** Internal padding between the component edge and its content. */
        float padding           = ThemeManager::get().spacing;

        /** Font used for labels and text content. */
        const sf::Font* font    = ThemeManager::get().font;

        /** Font size for primary text. */
        unsigned int fontSize   = ThemeManager::get().fontSize;

        /** Font size for secondary/description text. */
        unsigned int fontSizeSmall = ThemeManager::get().fontSizeSmall;

        void applyTheme(const Theme& theme) override
        {
            GraphicSettings::applyTheme(theme);
            hoverColor       = theme.secondary;
            focusColor       = theme.borderFocus;
            errorColor       = theme.error;
            disabledColor    = theme.disabled;
            disabledTextColor= theme.onDisabled;
            textColor        = theme.onSurface;
            mutedColor       = theme.muted;
            padding          = theme.spacing;
            font             = theme.font;
            fontSize         = theme.fontSize;
            fontSizeSmall    = theme.fontSizeSmall;
        }
    };

} // namespace ml

#endif // CONTROLSETTINGS_H
