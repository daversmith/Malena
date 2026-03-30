//
// ButtonSettings.h
//

#ifndef BUTTONSETTINGS_H
#define BUTTONSETTINGS_H

#pragma once

#include <Malena/Settings/ToggleSettings.h>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <string>

namespace ml
{
    /**
     * @brief Settings for the @c ButtonToggle (rectangular button that changes on toggle).
     * @ingroup Settings
     * @see ToggleSettings, ButtonToggle
     */
    struct ButtonSettings : ToggleSettings
    {
        sf::Vector2f size               = {100.f, 36.f};

        /** Button background color when ON. Defaults to @c Theme::primary. */
        sf::Color    onColor            = ThemeManager::get().primary;

        /** Button background color when OFF. Defaults to @c Theme::surface. */
        sf::Color    offColor           = ThemeManager::get().surface;

        /** Label text color when ON. Defaults to @c Theme::onPrimary. */
        sf::Color    onTextColor        = ThemeManager::get().onPrimary;

        /** Label text color when OFF. Defaults to @c Theme::onSurface. */
        sf::Color    offTextColor       = ThemeManager::get().onSurface;

        /** Border color. Defaults to @c Theme::border. */
        sf::Color    borderColor        = ThemeManager::get().border;

        float        borderThickness    = ThemeManager::get().borderThickness;
        float        buttonRadius       = ThemeManager::get().radius;

        /** Label shown when ON. */
        std::string  onLabel            = "ON";

        /** Label shown when OFF. */
        std::string  offLabel           = "OFF";

        // ── Icon support ──────────────────────────────────────────────────────

        const sf::Texture* iconOn       = nullptr;
        const sf::Texture* iconOff      = nullptr;
        sf::Vector2f       iconSize     = {0.f, 0.f}; ///< 0 = native size
        sf::IntRect        iconOnRect;
        sf::IntRect        iconOffRect;

        enum class IconAlign { ICON_ONLY, ICON_LEFT, ICON_RIGHT, ICON_ABOVE };
        IconAlign iconAlign     = IconAlign::ICON_ONLY;
        float     iconPadding   = 6.f;

        void applyTheme(const Theme& theme) override
        {
            ToggleSettings::applyTheme(theme);
            onColor          = theme.primary;
            offColor         = theme.surface;
            onTextColor      = theme.onPrimary;
            offTextColor     = theme.onSurface;
            borderColor      = theme.border;
            borderThickness  = theme.borderThickness;
            buttonRadius     = theme.radius;
        }
    };

} // namespace ml

#endif // BUTTONSETTINGS_H
