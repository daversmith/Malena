//
// RadioButtonSettings.h
//

#ifndef MALENA_RADIOBUTTONSETTINGS_H
#define MALENA_RADIOBUTTONSETTINGS_H

#pragma once

#include <Malena/Core/malena_export.h>
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
        float dotScale    = 0.5f;
        float labelOffset = 8.f;

        // ── Assignment from parent levels ─────────────────────────────────────

        RadioButtonSettings& operator=(const ControlSettings& c)
        { static_cast<ControlSettings&>(*this) = c; return *this; }

        RadioButtonSettings& operator=(const GraphicSettings& g)
        { static_cast<ControlSettings&>(*this) = g; return *this; }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] float getDotScale()    const { return dotScale; }
        [[nodiscard]] float getLabelOffset() const { return labelOffset; }

        void setDotScale(float s)    { dotScale    = s; }
        void setLabelOffset(float o) { labelOffset = o; }
    };

} // namespace ml

#endif // MALENA_RADIOBUTTONSETTINGS_H
