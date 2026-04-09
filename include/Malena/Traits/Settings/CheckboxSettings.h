//
// CheckboxSettings.h
//

#ifndef CHECKBOXSETTINGS_H
#define CHECKBOXSETTINGS_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/ControlSettings.h>

namespace ml
{
    /**
     * @brief Layout and behaviour settings for @c Checkbox.
     * @ingroup Traits
     *
     * Colors live in @c CheckboxTheme.
     *
     * @see ControlSettings, CheckboxTheme, CheckboxStyle, Checkbox
     */
    struct CheckboxSettings : ControlSettings
    {
        float boxSize     = 16.f;
        float boxRadius   = 2.f;
        float labelOffset = 8.f;

        // ── Assignment from parent levels ─────────────────────────────────────

        CheckboxSettings& operator=(const ControlSettings& c)
        { static_cast<ControlSettings&>(*this) = c; return *this; }

        CheckboxSettings& operator=(const GraphicSettings& g)
        { static_cast<ControlSettings&>(*this) = g; return *this; }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] float getBoxSize()     const { return boxSize; }
        [[nodiscard]] float getBoxRadius()   const { return boxRadius; }
        [[nodiscard]] float getLabelOffset() const { return labelOffset; }

        void setBoxSize(float s)     { boxSize     = s; }
        void setBoxRadius(float r)   { boxRadius   = r; }
        void setLabelOffset(float o) { labelOffset = o; }
    };

} // namespace ml

#endif // CHECKBOXSETTINGS_H
