//
// SelectStyle.h
//

#ifndef SELECTSTYLE_H
#define SELECTSTYLE_H

#pragma once

#include <Malena/Traits/Settings/SelectSettings.h>
#include <Malena/Traits/Theme/SelectTheme.h>

namespace ml
{
    /**
     * @brief Complete style descriptor for @c Select.
     * @ingroup Traits
     * @see SelectSettings, SelectTheme, Select
     */
    struct SelectStyle : SelectSettings, SelectTheme
    {
        SelectStyle& operator=(const SelectSettings& s)
        { static_cast<SelectSettings&>(*this) = s; return *this; }

        SelectStyle& operator=(const SelectTheme& t)
        { static_cast<SelectTheme&>(*this) = t; return *this; }

        SelectStyle& operator=(const InputTheme& i)
        { static_cast<SelectTheme&>(*this) = i; return *this; }

        SelectStyle& operator=(const InputSettings& i)
        { static_cast<SelectSettings&>(*this) = i; return *this; }

        SelectStyle& operator=(const ControlTheme& c)
        { static_cast<SelectTheme&>(*this) = c; return *this; }

        SelectStyle& operator=(const ControlSettings& c)
        { static_cast<SelectSettings&>(*this) = c; return *this; }

        SelectStyle& applyFrom(const Theme& t)
        { SelectTheme::applyFrom(t); return *this; }
    };

} // namespace ml

#endif // SELECTSTYLE_H
