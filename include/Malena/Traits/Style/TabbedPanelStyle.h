// TabbedPanelStyle.h
#ifndef TABBEDPANELSTYLE_H
#define TABBEDPANELSTYLE_H
#pragma once
#include <Malena/Traits/Settings/TabbedPanelSettings.h>
#include <Malena/Traits/Theme/TabbedPanelTheme.h>

namespace ml
{
    struct TabbedPanelStyle : TabbedPanelSettings, TabbedPanelTheme
    {
        TabbedPanelStyle& operator=(const TabbedPanelSettings& s)
        { static_cast<TabbedPanelSettings&>(*this) = s; return *this; }
        TabbedPanelStyle& operator=(const TabbedPanelTheme& t)
        { static_cast<TabbedPanelTheme&>(*this) = t; return *this; }
        TabbedPanelStyle& operator=(const ControlTheme& c)
        { static_cast<TabbedPanelTheme&>(*this) = c; return *this; }
        TabbedPanelStyle& operator=(const ControlSettings& c)
        { static_cast<TabbedPanelSettings&>(*this) = c; return *this; }
        TabbedPanelStyle& applyFrom(const Theme& t)
        { TabbedPanelTheme::applyFrom(t); return *this; }
    };
} // namespace ml
#endif
