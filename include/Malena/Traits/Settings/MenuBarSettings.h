// MenuBarSettings.h
#ifndef MALENA_MENUBARSETTINGS_H
#define MALENA_MENUBARSETTINGS_H
#pragma once
#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/ControlSettings.h>

namespace ml
{
    struct MenuBarSettings : ControlSettings
    {
        float  barHeight         = 30.f;
        float  entryPadding      = 14.f; ///< horizontal padding per entry label
        float  leftInset         = 0.f;  ///< extra space reserved on the left before entries
        float  dropdownItemH     = 28.f; ///< height of each dropdown row
        float  dropdownMinW      = 180.f;
        float  dropdownBorderThk = 1.f;
        bool   autoFillWidth     = true;

        MenuBarSettings& operator=(const ControlSettings& c)
        { static_cast<ControlSettings&>(*this) = c; return *this; }
        MenuBarSettings& operator=(const GraphicSettings& g)
        { static_cast<ControlSettings&>(*this) = g; return *this; }

        void setBarHeight(float h)         { barHeight         = h; }
        void setEntryPadding(float p)      { entryPadding      = p; }
        void setDropdownItemH(float h)     { dropdownItemH     = h; }
        void setDropdownMinW(float w)      { dropdownMinW      = w; }
        void setAutoFillWidth(bool b)      { autoFillWidth     = b; }
    };
} // namespace ml
#endif
