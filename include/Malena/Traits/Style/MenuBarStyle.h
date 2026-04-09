// MenuBarStyle.h
#ifndef MENUBARSTYLE_H
#define MENUBARSTYLE_H
#pragma once
#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/MenuBarSettings.h>
#include <Malena/Traits/Theme/MenuBarTheme.h>

namespace ml
{
    struct MenuBarStyle : MenuBarSettings, MenuBarTheme
    {
        MenuBarStyle& operator=(const MenuBarSettings& s)
        { static_cast<MenuBarSettings&>(*this) = s; return *this; }
        MenuBarStyle& operator=(const MenuBarTheme& t)
        { static_cast<MenuBarTheme&>(*this) = t; return *this; }
        MenuBarStyle& operator=(const ControlTheme& c)
        { static_cast<MenuBarTheme&>(*this) = c; return *this; }
        MenuBarStyle& operator=(const ControlSettings& c)
        { static_cast<MenuBarSettings&>(*this) = c; return *this; }
        MenuBarStyle& applyFrom(const Theme& t)
        { MenuBarTheme::applyFrom(t); return *this; }
    };
} // namespace ml
#endif
