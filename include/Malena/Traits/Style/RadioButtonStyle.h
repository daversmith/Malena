//
// RadioButtonStyle.h
//

#ifndef MALENA_RADIOBUTTONSTYLE_H
#define MALENA_RADIOBUTTONSTYLE_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/RadioButtonSettings.h>
#include <Malena/Traits/Theme/RadioButtonTheme.h>
#include <Malena/Traits/Settings/RadioGroupSettings.h>
#include <Malena/Traits/Theme/RadioGroupTheme.h>

namespace ml
{
    /**
     * @brief Complete style descriptor for @c RadioButton.
     * @ingroup Traits
     * @see RadioButtonSettings, RadioButtonTheme, RadioButton
     */
    struct RadioButtonStyle : RadioButtonSettings, RadioButtonTheme
    {
        RadioButtonStyle& operator=(const RadioButtonSettings& s)
        { static_cast<RadioButtonSettings&>(*this) = s; return *this; }

        RadioButtonStyle& operator=(const RadioButtonTheme& t)
        { static_cast<RadioButtonTheme&>(*this) = t; return *this; }

        RadioButtonStyle& operator=(const ControlTheme& c)
        { static_cast<RadioButtonTheme&>(*this) = c; return *this; }

        RadioButtonStyle& operator=(const ControlSettings& c)
        { static_cast<RadioButtonSettings&>(*this) = c; return *this; }

        RadioButtonStyle& applyFrom(const Theme& t)
        { RadioButtonTheme::applyFrom(t); return *this; }
    };

    /**
     * @brief Complete style descriptor for @c RadioGroup.
     * @ingroup Traits
     * @see RadioGroupSettings, RadioGroupTheme, RadioGroup
     */
    struct RadioGroupStyle : RadioGroupSettings, RadioGroupTheme
    {
        RadioGroupStyle& operator=(const RadioGroupSettings& s)
        { static_cast<RadioGroupSettings&>(*this) = s; return *this; }

        RadioGroupStyle& operator=(const RadioGroupTheme& t)
        { static_cast<RadioGroupTheme&>(*this) = t; return *this; }

        RadioGroupStyle& operator=(const RadioButtonSettings& s)
        { static_cast<RadioGroupSettings&>(*this) = s; return *this; }

        RadioGroupStyle& operator=(const RadioButtonTheme& t)
        { static_cast<RadioGroupTheme&>(*this) = t; return *this; }

        RadioGroupStyle& operator=(const ControlTheme& c)
        { static_cast<RadioGroupTheme&>(*this) = c; return *this; }

        RadioGroupStyle& operator=(const ControlSettings& c)
        { static_cast<RadioGroupSettings&>(*this) = c; return *this; }

        RadioGroupStyle& applyFrom(const Theme& t)
        { RadioGroupTheme::applyFrom(t); return *this; }
    };

} // namespace ml

#endif // MALENA_RADIOBUTTONSTYLE_H
