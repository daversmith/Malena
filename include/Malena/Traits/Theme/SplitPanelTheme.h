// SplitPanelTheme.h
#ifndef SPLITPANELTHEME_H
#define SPLITPANELTHEME_H
#pragma once
#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Theme/ControlTheme.h>

namespace ml
{
    struct SplitPanelTheme : ControlTheme
    {
        sf::Color dividerColor     = sf::Color(60,  60,  80);
        sf::Color dividerHover     = sf::Color(100, 100, 140);
        sf::Color dividerDrag      = sf::Color(70,  130, 230);
        sf::Color gripColor        = sf::Color(100, 100, 130);
        sf::Color paneBg           = sf::Color(28,  28,  38);

        SplitPanelTheme& applyFrom(const Theme& t) override
        {
            ControlTheme::applyFrom(t);
            dividerColor = t.border;
            dividerHover = sf::Color(
                std::min(255, t.border.r + 40),
                std::min(255, t.border.g + 40),
                std::min(255, t.border.b + 40));
            dividerDrag  = t.primary;
            gripColor    = t.muted;
            paneBg       = t.surface;
            return *this;
        }

        SplitPanelTheme& operator=(const ControlTheme& c)
        { static_cast<ControlTheme&>(*this) = c; return *this; }
        SplitPanelTheme& operator=(const GraphicTheme& g)
        { static_cast<ControlTheme&>(*this) = g; return *this; }
    };
} // namespace ml
#endif
