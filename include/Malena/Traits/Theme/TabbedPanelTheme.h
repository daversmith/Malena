// TabbedPanelTheme.h
#ifndef TABBEDPANELTHEME_H
#define TABBEDPANELTHEME_H
#pragma once
#include <Malena/Traits/Theme/ControlTheme.h>

namespace ml
{
    struct TabbedPanelTheme : ControlTheme
    {
        sf::Color tabBarBg        = sf::Color(25,  25,  32);
        sf::Color tabIdleBg       = sf::Color::Transparent;
        sf::Color tabHoverBg      = sf::Color(255, 255, 255, 15);
        sf::Color tabActiveBg     = sf::Color(35,  35,  48);
        sf::Color tabActiveBar    = sf::Color(70,  130, 230);  ///< accent line
        sf::Color tabIdleText     = sf::Color(160, 160, 180);
        sf::Color tabActiveText   = sf::Color(230, 230, 255);
        sf::Color contentBg       = sf::Color(35,  35,  48);
        sf::Color contentBorder   = sf::Color(60,  60,  80);
        sf::Color closeColor      = sf::Color(180, 80,  80);
        float     tabBarThickness = 1.f;  ///< divider between bar and content

        TabbedPanelTheme& applyFrom(const Theme& t) override
        {
            ControlTheme::applyFrom(t);
            tabBarBg      = sf::Color(
                std::max(0, t.surface.r - 10),
                std::max(0, t.surface.g - 10),
                std::max(0, t.surface.b - 10));
            tabActiveBg   = t.surface;
            tabActiveBar  = t.primary;
            tabIdleText   = t.muted;
            tabActiveText = t.onSurface;
            contentBg     = t.surface;
            contentBorder = t.border;
            closeColor    = t.error;
            return *this;
        }

        TabbedPanelTheme& operator=(const ControlTheme& c)
        { static_cast<ControlTheme&>(*this) = c; return *this; }
        TabbedPanelTheme& operator=(const GraphicTheme& g)
        { static_cast<ControlTheme&>(*this) = g; return *this; }
    };
} // namespace ml
#endif
