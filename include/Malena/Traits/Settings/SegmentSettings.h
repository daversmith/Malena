//
// SegmentSettings.h
//

#ifndef MALENA_SEGMENTSETTINGS_H
#define MALENA_SEGMENTSETTINGS_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/ToggleSettings.h>
#include <SFML/System/Vector2.hpp>
#include <string>

namespace ml
{
    /**
     * @brief Layout and behaviour settings for @c SegmentToggle.
     * @ingroup Traits
     *
     * Colors live in @c SegmentTheme.
     *
     * @see ToggleSettings, SegmentTheme, SegmentStyle, SegmentToggle
     */
    struct SegmentSettings : ToggleSettings
    {
        // ── Geometry ──────────────────────────────────────────────────────────
        sf::Vector2f size       = {200.f, 36.f};
        float        segRadius  = 20.f;
        float        segPadding = 3.f;

        // ── Labels ────────────────────────────────────────────────────────────
        std::string offLabel = "Off";
        std::string onLabel  = "On";

        // ── Assignment from parent levels ─────────────────────────────────────

        SegmentSettings& operator=(const ToggleSettings& t)
        {
            static_cast<ToggleSettings&>(*this) = t;
            return *this;
        }

        SegmentSettings& operator=(const ControlSettings& c)
        {
            static_cast<ToggleSettings&>(*this) = c;
            return *this;
        }

        SegmentSettings& operator=(const GraphicSettings& g)
        {
            static_cast<ToggleSettings&>(*this) = g;
            return *this;
        }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] sf::Vector2f getSize()       const { return size; }
        [[nodiscard]] float        getSegRadius()  const { return segRadius; }
        [[nodiscard]] float        getSegPadding() const { return segPadding; }
        [[nodiscard]] std::string  getOffLabel()   const { return offLabel; }
        [[nodiscard]] std::string  getOnLabel()    const { return onLabel; }

        void setSize(const sf::Vector2f& s)                                  { size       = s; }
        void setSegRadius(float r)                                           { segRadius  = r; }
        void setSegPadding(float p)                                          { segPadding = p; }
        void setOffLabel(const std::string& s)                               { offLabel   = s; }
        void setOnLabel(const std::string& s)                                { onLabel    = s; }
        void setSegmentLabels(const std::string& off, const std::string& on) { offLabel = off; onLabel = on; }
    };

} // namespace ml

#endif // MALENA_SEGMENTSETTINGS_H
