// TabbedPanelSettings.h
#ifndef TABBEDPANELSETTINGS_H
#define TABBEDPANELSETTINGS_H
#pragma once
#include <Malena/Traits/Settings/ControlSettings.h>

namespace ml
{
    struct TabbedPanelSettings : ControlSettings
    {
        enum class TabPosition { TOP, BOTTOM, LEFT, RIGHT };

        TabPosition  tabPosition   = TabPosition::TOP;
        float        tabHeight     = 36.f;  ///< thickness of the tab strip
        float        tabMinWidth   = 80.f;  ///< minimum tab label width
        float        tabMaxWidth   = 200.f; ///< maximum tab label width
        float        iconSize      = 16.f;  ///< icon square size
        bool         closeable     = false; ///< show × on all tabs by default
        float        closeSize     = 14.f;  ///< × hit area

        TabbedPanelSettings& operator=(const ControlSettings& c)
        { static_cast<ControlSettings&>(*this) = c; return *this; }
        TabbedPanelSettings& operator=(const GraphicSettings& g)
        { static_cast<ControlSettings&>(*this) = g; return *this; }

        void setTabPosition(TabPosition p) { tabPosition = p; }
        void setTabHeight(float h)         { tabHeight   = h; }
        void setTabMinWidth(float w)       { tabMinWidth = w; }
        void setTabMaxWidth(float w)       { tabMaxWidth = w; }
        void setIconSize(float s)          { iconSize    = s; }
        void setCloseable(bool b)          { closeable   = b; }
    };
} // namespace ml
#endif
