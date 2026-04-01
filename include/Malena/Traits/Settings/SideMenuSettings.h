// SideMenuSettings.h
#ifndef SIDEMENUSETTINGS_H
#define SIDEMENUSETTINGS_H
#pragma once
#include <Malena/Traits/Settings/ControlSettings.h>

namespace ml
{
    struct SideMenuSettings : ControlSettings
    {
        enum class Mode   { OVERLAY, PUSH };
        enum class Anchor { LEFT,    RIGHT };

        Mode   mode            = Mode::OVERLAY;
        Anchor anchor          = Anchor::LEFT;
        float  panelWidth      = 280.f;
        float  panelHeight     = 0.f;   ///< 0 = use window height
        float  animDuration    = 0.25f; ///< seconds
        float  hamburgerSize   = 48.f;  ///< hit-area square

        SideMenuSettings& operator=(const ControlSettings& c)
        { static_cast<ControlSettings&>(*this) = c; return *this; }
        SideMenuSettings& operator=(const GraphicSettings& g)
        { static_cast<ControlSettings&>(*this) = g; return *this; }

        void setMode(Mode m)            { mode          = m; }
        void setAnchor(Anchor a)        { anchor        = a; }
        void setPanelWidth(float w)     { panelWidth    = w; }
        void setPanelHeight(float h)    { panelHeight   = h; }
        void setAnimDuration(float d)   { animDuration  = d; }
        void setHamburgerSize(float s)  { hamburgerSize = s; }
    };
} // namespace ml
#endif
