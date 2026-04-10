//
// SelectTheme.h
//

#ifndef MALENA_SELECTTHEME_H
#define MALENA_SELECTTHEME_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Theme/InputTheme.h>

namespace ml
{
    /**
     * @brief Color tokens for @c Select.
     * @ingroup Traits
     *
     * Extends @c InputTheme with trigger hover/open states, arrow color,
     * and all dropdown panel colors.
     *
     * @see InputTheme, SelectSettings, SelectStyle, Select
     */
    struct SelectTheme : InputTheme
    {
        // ── Trigger ───────────────────────────────────────────────────────────
        sf::Color bgHovered       = sf::Color(55,  55,  55);
        sf::Color bgOpen          = sf::Color(50,  50,  50);
        sf::Color borderOpen      = sf::Color(70,  130, 230);
        sf::Color labelColor      = sf::Color::White;
        sf::Color labelDisabled   = sf::Color(80,  80,  80);
        sf::Color arrowColor      = sf::Color(180, 180, 180);

        // ── Panel ─────────────────────────────────────────────────────────────
        sf::Color panelBg            = sf::Color(45,  45,  45);
        sf::Color panelBorder        = sf::Color(80,  80,  80);
        sf::Color itemHoverBg        = sf::Color(70,  70,  70);
        sf::Color itemSelectedBg     = sf::Color(70,  130, 230, 80);
        sf::Color itemLabelColor     = sf::Color::White;
        sf::Color itemDescColor      = sf::Color(150, 150, 150);
        sf::Color itemDisabledColor  = sf::Color(80,  80,  80);
        float     panelBorderThick   = 1.f;

        // ── applyFrom ─────────────────────────────────────────────────────────

        SelectTheme& applyFrom(const Theme& t) override
        {
            InputTheme::applyFrom(t);
            bgHovered        = sf::Color(
                std::min(255, t.surface.r + 15),
                std::min(255, t.surface.g + 15),
                std::min(255, t.surface.b + 15));
            bgOpen           = sf::Color(
                std::min(255, t.surface.r + 10),
                std::min(255, t.surface.g + 10),
                std::min(255, t.surface.b + 10));
            borderOpen       = t.borderFocus;
            labelColor       = t.onSurface;
            labelDisabled    = t.onDisabled;
            arrowColor       = t.muted;
            panelBg          = sf::Color(
                std::min(255, t.surface.r + 5),
                std::min(255, t.surface.g + 5),
                std::min(255, t.surface.b + 5));
            panelBorder      = t.border;
            itemHoverBg      = t.secondary;
            itemSelectedBg   = sf::Color(t.primary.r, t.primary.g, t.primary.b, 80);
            itemLabelColor   = t.onSurface;
            itemDescColor    = t.muted;
            itemDisabledColor= t.onDisabled;
            return *this;
        }

        // ── Assignment from parent levels ─────────────────────────────────────

        SelectTheme& operator=(const InputTheme& i)
        { static_cast<InputTheme&>(*this) = i; return *this; }

        SelectTheme& operator=(const ControlTheme& c)
        { static_cast<InputTheme&>(*this) = c; return *this; }

        SelectTheme& operator=(const GraphicTheme& g)
        { static_cast<InputTheme&>(*this) = g; return *this; }

        // ── Getters / setters — trigger ───────────────────────────────────────

        [[nodiscard]] sf::Color getBgHovered()      const { return bgHovered; }
        [[nodiscard]] sf::Color getBgOpen()         const { return bgOpen; }
        [[nodiscard]] sf::Color getBorderOpen()     const { return borderOpen; }
        [[nodiscard]] sf::Color getLabelColor()     const { return labelColor; }
        [[nodiscard]] sf::Color getLabelDisabled()  const { return labelDisabled; }
        [[nodiscard]] sf::Color getArrowColor()     const { return arrowColor; }

        void setBgHovered(const sf::Color& c)      { bgHovered     = c; }
        void setBgOpen(const sf::Color& c)         { bgOpen        = c; }
        void setBorderOpen(const sf::Color& c)     { borderOpen    = c; }
        void setLabelColor(const sf::Color& c)     { labelColor    = c; }
        void setLabelDisabled(const sf::Color& c)  { labelDisabled = c; }
        void setArrowColor(const sf::Color& c)     { arrowColor    = c; }

        // ── Getters / setters — panel ─────────────────────────────────────────

        [[nodiscard]] sf::Color getPanelBg()            const { return panelBg; }
        [[nodiscard]] sf::Color getPanelBorder()        const { return panelBorder; }
        [[nodiscard]] sf::Color getItemHoverBg()        const { return itemHoverBg; }
        [[nodiscard]] sf::Color getItemSelectedBg()     const { return itemSelectedBg; }
        [[nodiscard]] sf::Color getItemLabelColor()     const { return itemLabelColor; }
        [[nodiscard]] sf::Color getItemDescColor()      const { return itemDescColor; }
        [[nodiscard]] sf::Color getItemDisabledColor()  const { return itemDisabledColor; }
        [[nodiscard]] float     getPanelBorderThick()   const { return panelBorderThick; }

        void setPanelBg(const sf::Color& c)            { panelBg           = c; }
        void setPanelBorder(const sf::Color& c)        { panelBorder       = c; }
        void setItemHoverBg(const sf::Color& c)        { itemHoverBg       = c; }
        void setItemSelectedBg(const sf::Color& c)     { itemSelectedBg    = c; }
        void setItemLabelColor(const sf::Color& c)     { itemLabelColor    = c; }
        void setItemDescColor(const sf::Color& c)      { itemDescColor     = c; }
        void setItemDisabledColor(const sf::Color& c)  { itemDisabledColor = c; }
        void setPanelBorderThick(float t)              { panelBorderThick  = t; }
    };

} // namespace ml

#endif // MALENA_SELECTTHEME_H
