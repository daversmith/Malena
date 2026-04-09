// MenuBarTheme.h
#ifndef MENUBARTHEME_H
#define MENUBARTHEME_H
#pragma once
#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Theme/ControlTheme.h>

namespace ml
{
    struct MenuBarTheme : ControlTheme
    {
        sf::Color barBg               = sf::Color(28,  28,  36);
        sf::Color entryHoverBg        = sf::Color(255, 255, 255, 20);
        sf::Color entryOpenBg         = sf::Color(255, 255, 255, 30);
        sf::Color dropdownBg          = sf::Color(35,  35,  45);
        sf::Color dropdownBorder      = sf::Color(70,  70,  90);
        sf::Color separatorColor      = sf::Color(70,  70,  90);
        sf::Color shortcutColor       = sf::Color(140, 140, 160);
        sf::Color checkmarkColor      = sf::Color(70,  130, 230);
        sf::Color itemHoverBg         = sf::Color(70,  130, 230, 60);
        sf::Color itemDisabledColor   = sf::Color(100, 100, 120);

        MenuBarTheme& applyFrom(const Theme& t) override
        {
            ControlTheme::applyFrom(t);
            barBg             = sf::Color(
                std::max(0, t.surface.r - 8),
                std::max(0, t.surface.g - 8),
                std::max(0, t.surface.b - 8));
            entryHoverBg      = sf::Color(t.onSurface.r, t.onSurface.g, t.onSurface.b, 20);
            entryOpenBg       = sf::Color(t.onSurface.r, t.onSurface.g, t.onSurface.b, 30);
            dropdownBg        = t.surface;
            dropdownBorder    = t.border;
            separatorColor    = t.border;
            shortcutColor     = t.muted;
            checkmarkColor    = t.primary;
            itemHoverBg       = sf::Color(t.primary.r, t.primary.g, t.primary.b, 60);
            itemDisabledColor = t.onDisabled;
            return *this;
        }
        MenuBarTheme& operator=(const ControlTheme& c)
        { static_cast<ControlTheme&>(*this) = c; return *this; }
        MenuBarTheme& operator=(const GraphicTheme& g)
        { static_cast<ControlTheme&>(*this) = g; return *this; }
    };
} // namespace ml
#endif
