// SplitPanelSettings.h
#ifndef SPLITPANELSETTINGS_H
#define SPLITPANELSETTINGS_H
#pragma once
#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/ControlSettings.h>

namespace ml
{
    struct SplitPanelSettings : ControlSettings
    {
        enum class Orientation { HORIZONTAL, VERTICAL };
        enum class HandleStyle { THIN, GRIP };  ///< thin line or visible grab handle

        Orientation  orientation    = Orientation::HORIZONTAL;
        HandleStyle  handleStyle    = HandleStyle::GRIP;
        float        dividerThick   = 4.f;   ///< draggable zone width/height
        float        minPaneSize    = 40.f;  ///< global minimum per pane
        bool         resizable      = true;

        SplitPanelSettings& operator=(const ControlSettings& c)
        { static_cast<ControlSettings&>(*this) = c; return *this; }
        SplitPanelSettings& operator=(const GraphicSettings& g)
        { static_cast<ControlSettings&>(*this) = g; return *this; }

        void setOrientation(Orientation o) { orientation  = o; }
        void setHandleStyle(HandleStyle s) { handleStyle  = s; }
        void setDividerThick(float t)      { dividerThick = t; }
        void setMinPaneSize(float s)       { minPaneSize  = s; }
        void setResizable(bool b)          { resizable    = b; }
    };
} // namespace ml
#endif
