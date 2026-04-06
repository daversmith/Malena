//
// PillSettings.h
//

#ifndef PILLSETTINGS_H
#define PILLSETTINGS_H

#pragma once

#include <Malena/Traits/Settings/ToggleSettings.h>
#include <SFML/System/Vector2.hpp>
#include <string>

namespace ml
{
    /**
     * @brief Layout and behaviour settings for @c PillToggle.
     * @ingroup Traits
     *
     * Colors live in @c PillTheme. @c PillSettings holds geometry and
     * content — track size, thumb margin, labels, and animation speed
     * (inherited from @c ToggleSettings).
     *
     * @see ToggleSettings, PillTheme, PillStyle, PillToggle
     */
    struct PillSettings : ToggleSettings
    {
        // ── Geometry ──────────────────────────────────────────────────────────
        sf::Vector2f trackSize   = {44.f, 24.f};
        float        thumbMargin = 2.f;
        float        labelOffset = 8.f;

        // ── Labels ────────────────────────────────────────────────────────────
        std::string pillOffLabel;
        std::string pillOnLabel;
        std::string leftLabel;
        std::string rightLabel;

        // ── Assignment from parent levels ─────────────────────────────────────

        PillSettings& operator=(const ToggleSettings& t)
        {
            static_cast<ToggleSettings&>(*this) = t;
            return *this;
        }

        PillSettings& operator=(const ControlSettings& c)
        {
            static_cast<ToggleSettings&>(*this) = c;
            return *this;
        }

        PillSettings& operator=(const GraphicSettings& g)
        {
            static_cast<ToggleSettings&>(*this) = g;
            return *this;
        }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Vector2f getTrackSize()    const { return trackSize; }
        [[nodiscard]] float        getThumbMargin()  const { return thumbMargin; }
        [[nodiscard]] float        getLabelOffset()  const { return labelOffset; }
        [[nodiscard]] std::string  getPillOffLabel() const { return pillOffLabel; }
        [[nodiscard]] std::string  getPillOnLabel()  const { return pillOnLabel; }
        [[nodiscard]] std::string  getLeftLabel()    const { return leftLabel; }
        [[nodiscard]] std::string  getRightLabel()   const { return rightLabel; }

        void setTrackSize(const sf::Vector2f& s)   { trackSize    = s; }
        void setThumbMargin(float m)               { thumbMargin  = m; }
        void setLabelOffset(float o)               { labelOffset  = o; }
        void setPillOffLabel(const std::string& s) { pillOffLabel = s; }
        void setPillOnLabel(const std::string& s)  { pillOnLabel  = s; }
        void setPillLabels(const std::string& off,
                           const std::string& on)  { pillOffLabel = off; pillOnLabel = on; }
        void setLeftLabel(const std::string& s)    { leftLabel    = s; }
        void setRightLabel(const std::string& s)   { rightLabel   = s; }
    };

} // namespace ml

#endif // PILLSETTINGS_H
