//
// TextInputSettings.h
//

#ifndef MALENA_TEXTINPUTSETTINGS_H
#define MALENA_TEXTINPUTSETTINGS_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/InputSettings.h>

namespace ml
{
    /**
     * @brief Layout and behaviour settings for @c TextInput and @c TextArea.
     * @ingroup Traits
     *
     * No extra fields over @c InputSettings — exists for naming clarity
     * and as a future extension point. Colors live in @c TextInputTheme.
     *
     * @see InputSettings, TextInputTheme, TextInputStyle, TextInput
     */
    struct TextInputSettings : InputSettings
    {
        // ── Assignment from parent levels ─────────────────────────────────────

        TextInputSettings& operator=(const InputSettings& i)
        { static_cast<InputSettings&>(*this) = i; return *this; }

        TextInputSettings& operator=(const ControlSettings& c)
        { static_cast<InputSettings&>(*this) = c; return *this; }

        TextInputSettings& operator=(const GraphicSettings& g)
        { static_cast<InputSettings&>(*this) = g; return *this; }
    };

} // namespace ml

#endif // MALENA_TEXTINPUTSETTINGS_H
