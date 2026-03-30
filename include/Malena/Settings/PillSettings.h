//
// PillSettings.h
//

#ifndef PILLSETTINGS_H
#define PILLSETTINGS_H

#pragma once

#include <Malena/Settings/ToggleSettings.h>
#include <SFML/System/Vector2.hpp>
#include <string>

namespace ml
{
    // ── PillSettings ─────────────────────────────────────────────────────────

    /**
     * @brief Settings for the @c PillToggle (iOS-style oval track with sliding thumb).
     * @ingroup Settings
     *
     * Extends @c ToggleSettings with all tokens specific to the pill switch style —
     * track size, thumb appearance, animation speed, and optional inside labels.
     *
     * ### Usage — default settings (reads active theme)
     * @code
     * ml::PillToggle t;  // constructed with PillSettings defaults
     * @endcode
     *
     * ### Usage — custom settings struct
     * @code
     * struct GreenPillSettings : ml::PillSettings
     * {
     *     GreenPillSettings()
     *     {
     *         trackOnColor = sf::Color(70, 200, 100);
     *         trackSize    = {50.f, 26.f};
     *         pillOnLabel  = "ON";
     *         pillOffLabel = "OFF";
     *     }
     * };
     *
     * t.applySettings(GreenPillSettings{});
     * @endcode
     *
     * ### Usage — inline override
     * @code
     * ml::PillSettings s;
     * s.trackOnColor = sf::Color(70, 200, 100);
     * s.items        = {{"Dark mode", "dark"}, {"Subtitles", "subs"}};
     * t.applySettings(s);
     * @endcode
     *
     * ### Usage — from manifest
     * @code
     * set(Settings::MainToggle, ml::PillSettings{
     *     .trackOnColor = sf::Color(70, 200, 100),
     *     .pillOnLabel  = "ON",
     *     .pillOffLabel = "OFF"
     * });
     *
     * t.applySettings(Resources::resource(Settings::MainToggle));
     * @endcode
     *
     * @see ToggleSettings, PillToggle
     */
    struct PillSettings : ToggleSettings
    {
        // ── Track ─────────────────────────────────────────────────────────────

        /** Width and height of the oval track. */
        sf::Vector2f trackSize    = {44.f, 24.f};

        /** Track fill color when the toggle is ON. Defaults to @c Theme::primary. */
        sf::Color    trackOnColor = ThemeManager::get().primary;

        /** Track fill color when the toggle is OFF. Defaults to @c Theme::border. */
        sf::Color    trackOffColor= ThemeManager::get().border;

        // ── Thumb ─────────────────────────────────────────────────────────────

        /** Thumb fill color. Defaults to @c Theme::onPrimary (white). */
        sf::Color    thumbColor   = ThemeManager::get().onPrimary;

        /** Gap between the thumb edge and the track edge in pixels. */
        float        thumbMargin  = 2.f;

        // ── Inside labels ─────────────────────────────────────────────────────

        /**
         * @brief Text drawn inside the track on the OFF (left) side.
         *
         * Leave empty for no inside label. Works alongside external
         * @c leftLabel / @c rightLabel.
         */
        std::string pillOffLabel;

        /**
         * @brief Text drawn inside the track on the ON (right) side.
         */
        std::string pillOnLabel;

        /** Color of the active inside label (the side matching current state). */
        sf::Color pillLabelActiveColor   = ThemeManager::get().onPrimary;

        /** Color of the inactive inside label. */
        sf::Color pillLabelInactiveColor = sf::Color(200, 200, 200, 180);

        // ── External labels ───────────────────────────────────────────────────

        /** Optional label shown to the left of the track. */
        std::string leftLabel;

        /** Optional label shown to the right of the track. */
        std::string rightLabel;

        /** Gap between the track edge and external labels. */
        float labelOffset = 8.f;

        // ── Animation ─────────────────────────────────────────────────────────

        // ── applyTheme ────────────────────────────────────────────────────────

        void applyTheme(const Theme& theme) override
        {
            ToggleSettings::applyTheme(theme);
            trackOnColor             = theme.primary;
            trackOffColor            = theme.border;
            thumbColor               = theme.onPrimary;
            pillLabelActiveColor     = theme.onPrimary;
        }
    };

} // namespace ml

#endif // PILLSETTINGS_H
