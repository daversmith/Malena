// ToolbarSettings.h
#ifndef MALENA_TOOLBARSETTINGS_H
#define MALENA_TOOLBARSETTINGS_H
#pragma once
#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/ControlSettings.h>
#include <SFML/System/Vector2.hpp>

namespace ml
{
    struct ToolbarSettings : ControlSettings
    {
        enum class Orientation { HORIZONTAL, VERTICAL };
        enum class Overflow    { CLIP, SCROLL };

        Orientation  orientation  = Orientation::HORIZONTAL;
        Overflow     overflow     = Overflow::CLIP;
        sf::Vector2f itemSize     = {36.f, 36.f}; ///< default size for created buttons
        float        itemSpacing  = 2.f;
        float        barPadding   = 4.f;

        ToolbarSettings& operator=(const ControlSettings& c)
        { static_cast<ControlSettings&>(*this) = c; return *this; }
        ToolbarSettings& operator=(const GraphicSettings& g)
        { static_cast<ControlSettings&>(*this) = g; return *this; }

        void setOrientation(Orientation o) { orientation = o; }
        void setOverflow(Overflow o)       { overflow    = o; }
        void setItemSize(sf::Vector2f s)   { itemSize    = s; }
        void setItemSpacing(float s)       { itemSpacing = s; }
        void setBarPadding(float p)        { barPadding  = p; }
    };
} // namespace ml
#endif
