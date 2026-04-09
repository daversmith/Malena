// SplitPanelStyle.h
#ifndef SPLITPANELSTYLE_H
#define SPLITPANELSTYLE_H
#pragma once
#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/SplitPanelSettings.h>
#include <Malena/Traits/Theme/SplitPanelTheme.h>

namespace ml
{
    struct SplitPanelStyle : SplitPanelSettings, SplitPanelTheme
    {
        SplitPanelStyle& operator=(const SplitPanelSettings& s)
        { static_cast<SplitPanelSettings&>(*this) = s; return *this; }
        SplitPanelStyle& operator=(const SplitPanelTheme& t)
        { static_cast<SplitPanelTheme&>(*this) = t; return *this; }
        SplitPanelStyle& operator=(const ControlTheme& c)
        { static_cast<SplitPanelTheme&>(*this) = c; return *this; }
        SplitPanelStyle& operator=(const ControlSettings& c)
        { static_cast<SplitPanelSettings&>(*this) = c; return *this; }
        SplitPanelStyle& applyFrom(const Theme& t)
        { SplitPanelTheme::applyFrom(t); return *this; }
    };
} // namespace ml
#endif
