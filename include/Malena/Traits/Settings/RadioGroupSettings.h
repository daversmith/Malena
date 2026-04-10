//
// RadioGroupSettings.h
//

#ifndef MALENA_RADIOGROUPSETTINGS_H
#define MALENA_RADIOGROUPSETTINGS_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/RadioButtonSettings.h>

namespace ml
{
    /**
     * @brief Layout and behaviour settings for @c RadioGroup.
     * @ingroup Traits
     *
     * Extends @c RadioButtonSettings with group-level geometry.
     * The inherited fields serve as defaults for every item the group creates.
     *
     * Colors live in @c RadioGroupTheme.
     *
     * @see RadioButtonSettings, RadioGroupTheme, RadioGroupStyle, RadioGroup
     */
    struct RadioGroupSettings : RadioButtonSettings
    {
        float spacing            = 28.f;
        float bgRadius           = 0.f;
        float bgOutlineThickness = 0.f;
        bool  showBackground     = false;

        // ── Assignment from parent levels ─────────────────────────────────────

        RadioGroupSettings& operator=(const RadioButtonSettings& r)
        { static_cast<RadioButtonSettings&>(*this) = r; return *this; }

        RadioGroupSettings& operator=(const ControlSettings& c)
        { static_cast<RadioButtonSettings&>(*this) = c; return *this; }

        RadioGroupSettings& operator=(const GraphicSettings& g)
        { static_cast<RadioButtonSettings&>(*this) = g; return *this; }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] float getSpacing()            const { return spacing; }
        [[nodiscard]] float getBgRadius()           const { return bgRadius; }
        [[nodiscard]] float getBgOutlineThickness() const { return bgOutlineThickness; }
        [[nodiscard]] bool  getShowBackground()     const { return showBackground; }

        void setSpacing(float s)            { spacing            = s; }
        void setBgRadius(float r)           { bgRadius           = r; }
        void setBgOutlineThickness(float t) { bgOutlineThickness = t; }
        void setShowBackground(bool b)      { showBackground     = b; }
    };

} // namespace ml

#endif // MALENA_RADIOGROUPSETTINGS_H
