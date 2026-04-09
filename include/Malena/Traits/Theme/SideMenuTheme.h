// SideMenuTheme.h
#ifndef SIDEMENUTHEME_H
#define SIDEMENUTHEME_H
#pragma once
#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Theme/ControlTheme.h>

namespace ml
{
    struct SideMenuTheme : ControlTheme
    {
        sf::Color panelBg        = sf::Color(28,  28,  36);
        sf::Color backdropColor  = sf::Color(0,   0,   0,  160);
        sf::Color hamburgerColor = sf::Color(220, 220, 220);
        sf::Color hamburgerBg    = sf::Color::Transparent;

        SideMenuTheme& applyFrom(const Theme& t) override
        {
            ControlTheme::applyFrom(t);
            panelBg        = sf::Color(
                std::max(0, t.surface.r - 8),
                std::max(0, t.surface.g - 8),
                std::max(0, t.surface.b - 8));
            hamburgerColor = t.onSurface;
            return *this;
        }
        SideMenuTheme& operator=(const ControlTheme& c)
        { static_cast<ControlTheme&>(*this) = c; return *this; }
        SideMenuTheme& operator=(const GraphicTheme& g)
        { static_cast<ControlTheme&>(*this) = g; return *this; }

        void setPanelBg(const sf::Color& c)       { panelBg        = c; }
        void setBackdropColor(const sf::Color& c)  { backdropColor  = c; }
        void setHamburgerColor(const sf::Color& c) { hamburgerColor = c; }
        void setHamburgerBg(const sf::Color& c)    { hamburgerBg    = c; }
    };
} // namespace ml
#endif
