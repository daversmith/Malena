#ifndef MALENA_MODALTHEME_H
#define MALENA_MODALTHEME_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Theme/ControlTheme.h>

namespace ml
{
    struct ModalTheme : ControlTheme
    {
        sf::Color backdropColor  = sf::Color(0,   0,   0,   160);
        sf::Color panelColor     = sf::Color(36,  36,  50);
        sf::Color titleTextColor = sf::Color::White;
        sf::Color bodyTextColor  = sf::Color(200, 200, 200);
        sf::Color separatorColor = sf::Color(60,  60,  80);
        sf::Color confirmColor   = sf::Color(0,   122, 255);
        sf::Color cancelColor    = sf::Color(70,  70,  70);
        sf::Color btnTextColor   = sf::Color::White;
        sf::Color closeBtnColor  = sf::Color(160, 160, 160);

        ModalTheme& applyFrom(const Theme& t) override
        {
            ControlTheme::applyFrom(t);
            panelColor     = sf::Color(
                std::max(0, t.surface.r - 6),
                std::max(0, t.surface.g - 6),
                std::max(0, t.surface.b - 6));
            titleTextColor = t.onSurface;
            bodyTextColor  = t.onSurface;
            separatorColor = t.border;
            confirmColor   = t.primary;
            btnTextColor   = t.onPrimary;
            closeBtnColor  = t.muted;
            return *this;
        }

        ModalTheme& operator=(const ControlTheme& c)
        { static_cast<ControlTheme&>(*this) = c; return *this; }

        void setBackdropColor(const sf::Color& c)  { backdropColor  = c; }
        void setPanelColor(const sf::Color& c)     { panelColor     = c; }
        void setTitleTextColor(const sf::Color& c) { titleTextColor = c; }
        void setBodyTextColor(const sf::Color& c)  { bodyTextColor  = c; }
        void setSeparatorColor(const sf::Color& c) { separatorColor = c; }
        void setConfirmColor(const sf::Color& c)   { confirmColor   = c; }
        void setCancelColor(const sf::Color& c)    { cancelColor    = c; }
        void setBtnTextColor(const sf::Color& c)   { btnTextColor   = c; }
        void setCloseBtnColor(const sf::Color& c)  { closeBtnColor  = c; }
    };

} // namespace ml

#endif // MALENA_MODALTHEME_H
