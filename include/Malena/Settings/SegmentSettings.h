//
// SegmentSettings.h
//

#ifndef SEGMENTSETTINGS_H
#define SEGMENTSETTINGS_H

#pragma once

#include <Malena/Settings/ToggleSettings.h>
#include <SFML/System/Vector2.hpp>
#include <string>

namespace ml
{
    /**
     * @brief Settings for the @c SegmentToggle (Hotels/Apartments style).
     * @ingroup Settings
     * @see ToggleSettings, SegmentToggle
     */
    struct SegmentSettings : ToggleSettings
    {
        sf::Vector2f size           = {200.f, 36.f};

        /** Label for the OFF (left) option. */
        std::string  offLabel       = "Off";

        /** Label for the ON (right) option. */
        std::string  onLabel        = "On";

        /** Background track color. */
        sf::Color    trackColor     = sf::Color(60, 60, 60);

        /** Sliding thumb/pill color. Defaults to @c Theme::primary. */
        sf::Color    thumbColor     = ThemeManager::get().primary;

        /** Text color of the active (selected) option. Defaults to @c Theme::onPrimary. */
        sf::Color    activeTextColor  = ThemeManager::get().onPrimary;

        /** Text color of the inactive option. Defaults to @c Theme::muted. */
        sf::Color    inactiveTextColor= ThemeManager::get().muted;

        /** Corner radius of the outer track. */
        float        segRadius      = 20.f;

        /** Padding between outer track edge and sliding thumb. */
        float        segPadding     = 3.f;

        void applyTheme(const Theme& theme) override
        {
            ToggleSettings::applyTheme(theme);
            thumbColor          = theme.primary;
            activeTextColor     = theme.onPrimary;
            inactiveTextColor   = theme.muted;
            trackColor          = theme.surface;
        }
    };

} // namespace ml

#endif // SEGMENTSETTINGS_H
