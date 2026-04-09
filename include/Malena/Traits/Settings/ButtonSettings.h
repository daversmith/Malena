//
// ButtonSettings.h
//

#ifndef BUTTONSETTINGS_H
#define BUTTONSETTINGS_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/ToggleSettings.h>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>

namespace ml
{
    /**
     * @brief Layout and behaviour settings for @c ButtonToggle.
     * @ingroup Traits
     *
     * Colors live in @c ButtonTheme.
     *
     * @see ToggleSettings, ButtonTheme, ButtonStyle, ButtonToggle
     */
    struct ButtonSettings : ToggleSettings
    {
        // ── Geometry ──────────────────────────────────────────────────────────
        sf::Vector2f size = {100.f, 36.f};

        // ── Labels ────────────────────────────────────────────────────────────
        std::string onLabel  = "ON";
        std::string offLabel = "OFF";

        // ── Icons ─────────────────────────────────────────────────────────────
        const sf::Texture* iconOn   = nullptr;
        const sf::Texture* iconOff  = nullptr;
        sf::Vector2f       iconSize = {0.f, 0.f}; ///< {0,0} = native size
        sf::IntRect        iconOnRect;
        sf::IntRect        iconOffRect;

        enum class IconAlign { ICON_ONLY, ICON_LEFT, ICON_RIGHT, ICON_ABOVE };
        IconAlign iconAlign   = IconAlign::ICON_ONLY;
        float     iconPadding = 6.f;

        // ── Assignment from parent levels ─────────────────────────────────────

        ButtonSettings& operator=(const ToggleSettings& t)
        {
            static_cast<ToggleSettings&>(*this) = t;
            return *this;
        }

        ButtonSettings& operator=(const ControlSettings& c)
        {
            static_cast<ToggleSettings&>(*this) = c;
            return *this;
        }

        ButtonSettings& operator=(const GraphicSettings& g)
        {
            static_cast<ToggleSettings&>(*this) = g;
            return *this;
        }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Vector2f       getSize()        const { return size; }
        [[nodiscard]] std::string        getOnLabel()     const { return onLabel; }
        [[nodiscard]] std::string        getOffLabel()    const { return offLabel; }
        [[nodiscard]] const sf::Texture* getIconOn()      const { return iconOn; }
        [[nodiscard]] const sf::Texture* getIconOff()     const { return iconOff; }
        [[nodiscard]] sf::Vector2f       getIconSize()    const { return iconSize; }
        [[nodiscard]] sf::IntRect        getIconOnRect()  const { return iconOnRect; }
        [[nodiscard]] sf::IntRect        getIconOffRect() const { return iconOffRect; }
        [[nodiscard]] IconAlign          getIconAlign()   const { return iconAlign; }
        [[nodiscard]] float              getIconPadding() const { return iconPadding; }

        void setSize(const sf::Vector2f& s)      { size        = s; }
        void setOnLabel(const std::string& s)    { onLabel     = s; }
        void setOffLabel(const std::string& s)   { offLabel    = s; }
        void setLabels(const std::string& on,
                       const std::string& off)   { onLabel = on; offLabel = off; }
        void setIcons(const sf::Texture* on,
                      const sf::Texture* off = nullptr)
        { iconOn = on; iconOff = off ? off : on; }
        void setIconOn(const sf::Texture* t)     { iconOn      = t; }
        void setIconOff(const sf::Texture* t)    { iconOff     = t; }
        void setIconSize(const sf::Vector2f& s)  { iconSize    = s; }
        void setIconOnRect(const sf::IntRect& r) { iconOnRect  = r; }
        void setIconOffRect(const sf::IntRect& r){ iconOffRect = r; }
        void setIconAlign(IconAlign a)           { iconAlign   = a; }
        void setIconPadding(float p)             { iconPadding = p; }
    };

} // namespace ml

#endif // BUTTONSETTINGS_H
