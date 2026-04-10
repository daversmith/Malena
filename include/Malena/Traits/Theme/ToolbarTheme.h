// ToolbarTheme.h
#ifndef MALENA_TOOLBARTHEME_H
#define MALENA_TOOLBARTHEME_H
#pragma once
#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Theme/ControlTheme.h>

namespace ml
{
    struct ToolbarTheme : ControlTheme
    {
        sf::Color barBg          = sf::Color(28, 28, 36);
        sf::Color separatorColor = sf::Color(70, 70, 90);
        sf::Color itemHoverBg    = sf::Color(255, 255, 255, 20);
        sf::Color itemActiveBg   = sf::Color(70, 130, 230, 80);

        ToolbarTheme& applyFrom(const Theme& t) override
        {
            ControlTheme::applyFrom(t);
            barBg          = sf::Color(
                std::max(0, t.surface.r - 8),
                std::max(0, t.surface.g - 8),
                std::max(0, t.surface.b - 8));
            separatorColor = t.border;
            itemHoverBg    = sf::Color(t.onSurface.r, t.onSurface.g, t.onSurface.b, 20);
            itemActiveBg   = sf::Color(t.primary.r, t.primary.g, t.primary.b, 80);
            return *this;
        }
        ToolbarTheme& operator=(const ControlTheme& c)
        { static_cast<ControlTheme&>(*this) = c; return *this; }
        ToolbarTheme& operator=(const GraphicTheme& g)
        { static_cast<ControlTheme&>(*this) = g; return *this; }

        void setBarBg(const sf::Color& c)          { barBg          = c; }
        void setSeparatorColor(const sf::Color& c) { separatorColor = c; }
        void setItemHoverBg(const sf::Color& c)    { itemHoverBg    = c; }
        void setItemActiveBg(const sf::Color& c)   { itemActiveBg   = c; }
    };
} // namespace ml
#endif
