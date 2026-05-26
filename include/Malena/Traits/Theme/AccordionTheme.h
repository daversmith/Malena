
#ifndef MALENA_ACCORDIONTHEME_H
#define MALENA_ACCORDIONTHEME_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Theme/ControlTheme.h>

namespace ml
{
    struct AccordionTheme : ControlTheme
    {
        sf::Color headerBg      = sf::Color(45,  45,  45);
        sf::Color headerHovered = sf::Color(60,  60,  60);
        sf::Color contentBg     = sf::Color(30,  30,  30);
        sf::Color chevronColor  = sf::Color(180, 180, 180);
        sf::Color dividerColor  = sf::Color(70,  70,  70);

        AccordionTheme& applyFrom(const Theme& t) override
        {
            ControlTheme::applyFrom(t);
            headerBg      = t.surface;
            headerHovered = t.secondary;
            contentBg     = t.background;
            chevronColor  = t.onSurface;
            dividerColor  = t.border;
            return *this;
        }

        AccordionTheme& operator=(const ControlTheme& c)
        { static_cast<ControlTheme&>(*this) = c; return *this; }

        AccordionTheme& operator=(const GraphicTheme& g)
        { static_cast<ControlTheme&>(*this) = g; return *this; }

        [[nodiscard]] sf::Color getHeaderBg()      const { return headerBg;      }
        [[nodiscard]] sf::Color getHeaderHovered()  const { return headerHovered; }
        [[nodiscard]] sf::Color getContentBg()      const { return contentBg;     }
        [[nodiscard]] sf::Color getChevronColor()   const { return chevronColor;  }
        [[nodiscard]] sf::Color getDividerColor()   const { return dividerColor;  }

        void setHeaderBg(const sf::Color& c)      { headerBg      = c; }
        void setHeaderHovered(const sf::Color& c) { headerHovered = c; }
        void setContentBg(const sf::Color& c)     { contentBg     = c; }
        void setChevronColor(const sf::Color& c)  { chevronColor  = c; }
        void setDividerColor(const sf::Color& c)  { dividerColor  = c; }
    };

} // namespace ml

#endif // MALENA_ACCORDIONTHEME_H
