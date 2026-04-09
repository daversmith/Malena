// ToolbarStyle.h
#ifndef TOOLBARSTYLE_H
#define TOOLBARSTYLE_H
#pragma once
#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/ToolbarSettings.h>
#include <Malena/Traits/Theme/ToolbarTheme.h>

namespace ml
{
    struct ToolbarStyle : ToolbarSettings, ToolbarTheme
    {
        ToolbarStyle& operator=(const ToolbarSettings& s)
        { static_cast<ToolbarSettings&>(*this) = s; return *this; }
        ToolbarStyle& operator=(const ToolbarTheme& t)
        { static_cast<ToolbarTheme&>(*this) = t; return *this; }
        ToolbarStyle& operator=(const ControlTheme& c)
        { static_cast<ToolbarTheme&>(*this) = c; return *this; }
        ToolbarStyle& operator=(const ControlSettings& c)
        { static_cast<ToolbarSettings&>(*this) = c; return *this; }
        ToolbarStyle& applyFrom(const Theme& t)
        { ToolbarTheme::applyFrom(t); return *this; }
    };
} // namespace ml
#endif
