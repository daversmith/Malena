//
// GraphicSettings.h
//

#ifndef GRAPHICSETTINGS_H
#define GRAPHICSETTINGS_H

#pragma once

#include <Malena/Manifests/Theme.h>
#include <Malena/Resources/ThemeManager.h>
#include <SFML/Graphics/Color.hpp>

namespace ml
{
    // ── GraphicSettings ───────────────────────────────────────────────────────

    /**
     * @brief Base settings for all visual components.
     * @ingroup Settings
     *
     * @c GraphicSettings defines the lowest-level visual tokens shared by
     * every drawable component — fill color, outline, and corner radius.
     * All other settings structs inherit from this.
     *
     * Theme tokens are read from @c ThemeManager::get() at construction time
     * and updated automatically via @c applyTheme() when the global theme changes.
     *
     * ### Inheritance chain
     * @code
     * GraphicSettings
     *     └── ControlSettings
     *             └── ToggleSettings
     *                     └── PillSettings
     *                     └── SegmentSettings
     *                     └── ButtonSettings
     * @endcode
     *
     * ### Direct use
     * @code
     * struct MySettings : ml::GraphicSettings
     * {
     *     MySettings()
     *     {
     *         fillColor = sf::Color(30, 30, 50);  // override one token
     *     }
     * };
     * @endcode
     *
     * @see ControlSettings, ThemeManager, Theme
     */
    struct GraphicSettings
    {
        /** Background fill color. Defaults to @c Theme::surface. */
        sf::Color fillColor     = ThemeManager::get().surface;

        /** Outline/border color. Defaults to @c Theme::border. */
        sf::Color outlineColor  = ThemeManager::get().border;

        /** Outline thickness in pixels. */
        float outlineThickness  = ThemeManager::get().borderThickness;

        /** Corner radius for rounded shapes. Defaults to @c Theme::radius. */
        float radius            = ThemeManager::get().radius;

        /**
         * @brief Update all theme-derived tokens from the given theme.
         *
         * Called automatically by @c Themeable::onThemeApplied().
         * Subclasses must call @c GraphicSettings::applyTheme(theme) first
         * before updating their own tokens:
         *
         * @code
         * void ControlSettings::applyTheme(const Theme& t)
         * {
         *     GraphicSettings::applyTheme(t);  // always chain up
         *     hoverColor = t.secondary;
         * }
         * @endcode
         *
         * @param theme  The newly active theme.
         */
        virtual void applyTheme(const Theme& theme)
        {
            fillColor          = theme.surface;
            outlineColor       = theme.border;
            outlineThickness   = theme.borderThickness;
            radius             = theme.radius;
        }

        virtual ~GraphicSettings() = default;
    };

} // namespace ml

#endif // GRAPHICSETTINGS_H
