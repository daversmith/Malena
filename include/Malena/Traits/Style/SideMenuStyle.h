// SideMenuStyle.h
#ifndef MALENA_SIDEMENUSTYLE_H
#define MALENA_SIDEMENUSTYLE_H
#pragma once
#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/SideMenuSettings.h>
#include <Malena/Traits/Theme/SideMenuTheme.h>

namespace ml
{
    struct SideMenuStyle : SideMenuSettings, SideMenuTheme
    {
        SideMenuStyle& operator=(const SideMenuSettings& s)
        { static_cast<SideMenuSettings&>(*this) = s; return *this; }
        SideMenuStyle& operator=(const SideMenuTheme& t)
        { static_cast<SideMenuTheme&>(*this) = t; return *this; }
        SideMenuStyle& operator=(const ControlTheme& c)
        { static_cast<SideMenuTheme&>(*this) = c; return *this; }
        SideMenuStyle& operator=(const ControlSettings& c)
        { static_cast<SideMenuSettings&>(*this) = c; return *this; }
        SideMenuStyle& applyFrom(const Theme& t)
        { SideMenuTheme::applyFrom(t); return *this; }
    };
} // namespace ml
#endif
