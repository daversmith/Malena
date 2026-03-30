//
// ToggleSettings.h
//

#ifndef TOGGLESETTINGS_H
#define TOGGLESETTINGS_H

#pragma once

#include <Malena/Settings/ControlSettings.h>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>
#include <utility>

namespace ml
{
    // ── ToggleSettings ────────────────────────────────────────────────────────

    /**
     * @brief Shared settings for all toggle variants.
     * @ingroup Settings
     *
     * Extends @c ControlSettings with properties common to every toggle style —
     * on/off labels, active color, and a pre-population list for @c ToggleGroup.
     *
     * Each style-specific settings struct (@c PillSettings, @c SegmentSettings,
     * @c ButtonSettings) inherits from @c ToggleSettings and adds its own tokens.
     *
     * ### Pre-populating a ToggleGroup via manifest settings
     * @code
     * set(Settings::SoundToggles, ml::ToggleSettings{
     *     .items = {
     *         {"Music",   "music"},
     *         {"SFX",     "sfx"},
     *         {"Ambient", "ambient"}
     *     }
     * });
     * @endcode
     *
     * @see ControlSettings, PillSettings, SegmentSettings, ButtonSettings
     */
    struct ToggleSettings : ControlSettings
    {
        /** Color of the active/on state. Defaults to @c Theme::primary. */
        sf::Color activeColor   = ThemeManager::get().primary;

        /** Color of the inactive/off state. Defaults to @c Theme::surface. */
        sf::Color inactiveColor = ThemeManager::get().surface;

        /** Text color on top of the active state. Defaults to @c Theme::onPrimary. */
        sf::Color activeTextColor = ThemeManager::get().onPrimary;

        /**
         * @brief Pre-population list for @c ToggleGroup.
         *
         * Each pair is { display label, logical key }.
         * If non-empty, the group is populated with these items when
         * @c applySettings() is called.
         *
         * @code
         * .items = {{"Dark mode", "dark"}, {"Subtitles", "subs"}}
         * @endcode
         */
        std::vector<std::pair<std::string, std::string>> items;

        /**
         * @brief Lerp speed of any sliding animation.
         * Higher = faster snap. 0 = instant. Defaults to 10.
         */
        float animSpeed = 10.f;

        void applyTheme(const Theme& theme) override
        {
            ControlSettings::applyTheme(theme);
            activeColor      = theme.primary;
            inactiveColor    = theme.surface;
            activeTextColor  = theme.onPrimary;
        }
    };

} // namespace ml

#endif // TOGGLESETTINGS_H
