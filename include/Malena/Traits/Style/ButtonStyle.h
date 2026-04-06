//
// ButtonStyle.h
//

#ifndef BUTTONSTYLE_H
#define BUTTONSTYLE_H

#pragma once

#include <Malena/Traits/Settings/ButtonSettings.h>
#include <Malena/Traits/Theme/ButtonTheme.h>

namespace ml
{
    /**
     * @brief Complete style descriptor for @c ButtonToggle.
     * @ingroup Traits
     *
     * Combines @c ButtonSettings and @c ButtonTheme.
     *
     * @see ButtonSettings, ButtonTheme, ButtonToggle
     */
    struct ButtonStyle : ButtonSettings, ButtonTheme
    {
        ButtonStyle& operator=(const ButtonSettings& s)
        { static_cast<ButtonSettings&>(*this) = s; return *this; }

        ButtonStyle& operator=(const ButtonTheme& t)
        { static_cast<ButtonTheme&>(*this) = t; return *this; }

        ButtonStyle& operator=(const ToggleTheme& t)
        { static_cast<ButtonTheme&>(*this) = t; return *this; }

        ButtonStyle& operator=(const ControlTheme& c)
        { static_cast<ButtonTheme&>(*this) = c; return *this; }

        ButtonStyle& operator=(const GraphicTheme& g)
        { static_cast<ButtonTheme&>(*this) = g; return *this; }

        ButtonStyle& operator=(const ToggleSettings& t)
        { static_cast<ButtonSettings&>(*this) = t; return *this; }

        ButtonStyle& operator=(const ControlSettings& c)
        { static_cast<ButtonSettings&>(*this) = c; return *this; }

        ButtonStyle& applyFrom(const Theme& t)
        { ButtonTheme::applyFrom(t); return *this; }
    };

} // namespace ml

#endif // BUTTONSTYLE_H
