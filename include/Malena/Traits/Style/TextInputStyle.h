//
// TextInputStyle.h
//

#ifndef TEXTINPUTSTYLE_H
#define TEXTINPUTSTYLE_H

#pragma once

#include <Malena/Traits/Settings/TextInputSettings.h>
#include <Malena/Traits/Theme/TextInputTheme.h>
#include <Malena/Traits/Theme/TextAreaTheme.h>

namespace ml
{
    /**
     * @brief Complete style descriptor for @c TextInput.
     * @ingroup Traits
     * @see TextInputSettings, TextInputTheme, TextInput
     */
    struct TextInputStyle : TextInputSettings, TextInputTheme
    {
        TextInputStyle& operator=(const TextInputSettings& s)
        { static_cast<TextInputSettings&>(*this) = s; return *this; }

        TextInputStyle& operator=(const TextInputTheme& t)
        { static_cast<TextInputTheme&>(*this) = t; return *this; }

        TextInputStyle& operator=(const InputTheme& i)
        { static_cast<TextInputTheme&>(*this) = i; return *this; }

        TextInputStyle& operator=(const InputSettings& i)
        { static_cast<TextInputSettings&>(*this) = i; return *this; }

        TextInputStyle& operator=(const ControlTheme& c)
        { static_cast<TextInputTheme&>(*this) = c; return *this; }

        TextInputStyle& operator=(const ControlSettings& c)
        { static_cast<TextInputSettings&>(*this) = c; return *this; }

        TextInputStyle& applyFrom(const Theme& t)
        { TextInputTheme::applyFrom(t); return *this; }
    };

    /**
     * @brief Complete style descriptor for @c TextArea.
     * @ingroup Traits
     *
     * Uses @c TextInputSettings (not TextAreaSettings) for the settings
     * layer — TextArea scrollbar width lives in @c TextAreaTheme —
     * to avoid diamond inheritance since @c TextArea extends @c TextInput.
     *
     * @see TextInputSettings, TextAreaTheme, TextArea
     */
    struct TextAreaStyle : TextInputSettings, TextAreaTheme
    {
        TextAreaStyle& operator=(const TextInputSettings& s)
        { static_cast<TextInputSettings&>(*this) = s; return *this; }

        TextAreaStyle& operator=(const TextAreaTheme& t)
        { static_cast<TextAreaTheme&>(*this) = t; return *this; }

        TextAreaStyle& operator=(const TextInputTheme& t)
        { static_cast<TextAreaTheme&>(*this) = t; return *this; }

        TextAreaStyle& operator=(const InputTheme& i)
        { static_cast<TextAreaTheme&>(*this) = i; return *this; }

        TextAreaStyle& operator=(const InputSettings& i)
        { static_cast<TextInputSettings&>(*this) = i; return *this; }

        TextAreaStyle& operator=(const ControlTheme& c)
        { static_cast<TextAreaTheme&>(*this) = c; return *this; }

        TextAreaStyle& operator=(const ControlSettings& c)
        { static_cast<TextInputSettings&>(*this) = c; return *this; }

        TextAreaStyle& applyFrom(const Theme& t)
        { TextAreaTheme::applyFrom(t); return *this; }
    };

} // namespace ml

#endif // TEXTINPUTSTYLE_H
