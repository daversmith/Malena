//
// RadioButtonSettings.h
//

#ifndef RADIOBUTTONSETTINGS_H
#define RADIOBUTTONSETTINGS_H

#pragma once

#include <Malena/Traits/Settings/ControlSettings.h>

namespace ml
{
    /**
     * @brief Layout and behaviour settings for @c RadioButton.
     * @ingroup Traits
     *
     * Colors live in @c RadioButtonTheme.
     *
     * @see ControlSettings, RadioButtonTheme, RadioButtonStyle, RadioButton
     */
    struct RadioButtonSettings : ControlSettings
    {
        float radius      = 10.f;
        float dotScale    = 0.5f;
        float labelOffset = 8.f;

        // ── Assignment from parent levels ─────────────────────────────────────

        RadioButtonSettings& operator=(const ControlSettings& c)
        { static_cast<ControlSettings&>(*this) = c; return *this; }

        RadioButtonSettings& operator=(const GraphicSettings& g)
        { static_cast<ControlSettings&>(*this) = g; return *this; }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] float getRadius()      const { return radius; }
        [[nodiscard]] float getDotScale()    const { return dotScale; }
        [[nodiscard]] float getLabelOffset() const { return labelOffset; }

        void setRadius(float r)      { radius      = r; }
        void setDotScale(float s)    { dotScale    = s; }
        void setLabelOffset(float o) { labelOffset = o; }
    };

} // namespace ml

#endif // RADIOBUTTONSETTINGS_H
