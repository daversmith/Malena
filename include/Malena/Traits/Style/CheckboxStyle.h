//
// CheckboxStyle.h
//

#ifndef MALENA_CHECKBOXSTYLE_H
#define MALENA_CHECKBOXSTYLE_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/CheckboxSettings.h>
#include <Malena/Traits/Theme/CheckboxTheme.h>

namespace ml
{
    /**
     * @brief Complete style descriptor for @c Checkbox.
     * @ingroup Traits
     *
     * Combines @c CheckboxSettings (geometry/behaviour) and
     * @c CheckboxTheme (colors/font) into one struct for one-stop
     * style application.
     *
     * @see CheckboxSettings, CheckboxTheme, Checkbox
     */
    struct CheckboxStyle : CheckboxSettings, CheckboxTheme
    {
        CheckboxStyle& operator=(const CheckboxSettings& s)
        { static_cast<CheckboxSettings&>(*this) = s; return *this; }

        CheckboxStyle& operator=(const CheckboxTheme& t)
        { static_cast<CheckboxTheme&>(*this) = t; return *this; }

        CheckboxStyle& operator=(const ControlTheme& c)
        { static_cast<CheckboxTheme&>(*this) = c; return *this; }

        CheckboxStyle& operator=(const ControlSettings& c)
        { static_cast<CheckboxSettings&>(*this) = c; return *this; }

        CheckboxStyle& applyFrom(const Theme& t)
        { CheckboxTheme::applyFrom(t); return *this; }
    };

    struct CheckboxGroupStyle : CheckboxGroupSettings, CheckboxGroupTheme
    {
        CheckboxGroupStyle& operator=(const CheckboxGroupSettings& s)
        { static_cast<CheckboxGroupSettings&>(*this) = s; return *this; }

        CheckboxGroupStyle& operator=(const CheckboxGroupTheme& t)
        { static_cast<CheckboxGroupTheme&>(*this) = t; return *this; }

        CheckboxGroupStyle& operator=(const CheckboxSettings& s)
        { static_cast<CheckboxGroupSettings&>(*this) = s; return *this; }

        CheckboxGroupStyle& operator=(const CheckboxTheme& t)
        { static_cast<CheckboxGroupTheme&>(*this) = t; return *this; }

        CheckboxGroupStyle& operator=(const ControlTheme& c)
        { static_cast<CheckboxGroupTheme&>(*this) = c; return *this; }

        CheckboxGroupStyle& operator=(const ControlSettings& c)
        { static_cast<CheckboxGroupSettings&>(*this) = c; return *this; }

        CheckboxGroupStyle& applyFrom(const Theme& t)
        { CheckboxGroupTheme::applyFrom(t); return *this; }
    };

} // namespace ml

#endif // MALENA_CHECKBOXSTYLE_H
