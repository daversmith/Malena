// MenuBarSettings.h
#ifndef MENUBARSETTINGS_H
#define MENUBARSETTINGS_H
#pragma once
#include <Malena/Traits/Settings/ControlSettings.h>

namespace ml
{
    struct MenuBarSettings : ControlSettings
    {
        float  barHeight         = 30.f;
        float  entryPadding      = 14.f; ///< horizontal padding per entry label
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
