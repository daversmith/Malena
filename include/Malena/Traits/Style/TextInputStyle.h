//
// TextInputStyle.h
//

#ifndef MALENA_TEXTINPUTSTYLE_H
#define MALENA_TEXTINPUTSTYLE_H

#pragma once

#include <Malena/Core/malena_export.h>
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
     * Combines @c TextInputSettings (geometry), @c TextInputTheme (text/cursor/
     * border colors), and @c TextAreaTheme (scrollbar colors) into one struct.
     *
     * Note: @c TextAreaTheme is standalone (no base classes) to avoid diamond
     * inheritance when used in the @c TextArea component hierarchy.
     *
     * @see TextInputSettings, TextInputTheme, TextAreaTheme, TextArea
     */
    struct TextAreaStyle : TextInputSettings, TextInputTheme, TextAreaTheme
    {
        TextAreaStyle& operator=(const TextInputSettings& s)
        { static_cast<TextInputSettings&>(*this) = s; return *this; }

        TextAreaStyle& operator=(const TextInputTheme& t)
        { static_cast<TextInputTheme&>(*this) = t; return *this; }

        TextAreaStyle& operator=(const TextAreaTheme& t)
        { static_cast<TextAreaTheme&>(*this) = t; return *this; }

        TextAreaStyle& operator=(const InputTheme& i)
        { static_cast<TextInputTheme&>(*this) = i; return *this; }

        TextAreaStyle& operator=(const InputSettings& i)
        { static_cast<TextInputSettings&>(*this) = i; return *this; }

        TextAreaStyle& operator=(const ControlTheme& c)
        { static_cast<TextInputTheme&>(*this) = c; return *this; }

        TextAreaStyle& operator=(const ControlSettings& c)
        { static_cast<TextInputSettings&>(*this) = c; return *this; }

        TextAreaStyle& applyFrom(const Theme& t)
        {
            TextInputTheme::applyFrom(t);
            TextAreaTheme::applyFrom(t);
            return *this;
        }
    };

} // namespace ml

#endif // MALENA_TEXTINPUTSTYLE_H
