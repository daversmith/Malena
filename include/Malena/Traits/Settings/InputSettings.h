//
// InputSettings.h
//

#ifndef INPUTSETTINGS_H
#define INPUTSETTINGS_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/ControlSettings.h>
#include <SFML/System/Vector2.hpp>
#include <cstddef>

namespace ml
{
    /**
     * @brief Layout and behaviour settings shared by all input controls.
     * @ingroup Traits
     *
     * Base for @c TextInputSettings and @c SelectSettings.
     * Colors live in @c InputTheme.
     *
     * @see ControlSettings, InputTheme, TextInputSettings, SelectSettings
     */
    struct InputSettings : ControlSettings
    {
        sf::Vector2f size         = {200.f, 36.f};
        std::size_t  maxLength    = 0;         ///< 0 = unlimited
        bool         passwordMode = false;
        char32_t     passwordChar = U'\u2022'; ///< Bullet •

        // ── Assignment from parent levels ─────────────────────────────────────

        InputSettings& operator=(const ControlSettings& c)
        { static_cast<ControlSettings&>(*this) = c; return *this; }

        InputSettings& operator=(const GraphicSettings& g)
        { static_cast<ControlSettings&>(*this) = g; return *this; }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Vector2f getSize()         const { return size; }
        [[nodiscard]] std::size_t  getMaxLength()     const { return maxLength; }
        [[nodiscard]] bool         isPasswordMode()   const { return passwordMode; }
        [[nodiscard]] char32_t     getPasswordChar()  const { return passwordChar; }

        void setSize(const sf::Vector2f& s) { size         = s; }
        void setMaxLength(std::size_t n)    { maxLength    = n; }
        void setPasswordMode(bool b)        { passwordMode = b; }
        void setPasswordChar(char32_t c)    { passwordChar = c; }
    };

} // namespace ml

#endif // INPUTSETTINGS_H
