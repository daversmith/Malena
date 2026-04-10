//
// SelectSettings.h
//

#ifndef MALENA_SELECTSETTINGS_H
#define MALENA_SELECTSETTINGS_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/InputSettings.h>

namespace ml
{
    /**
     * @brief Layout and behaviour settings for @c Select.
     * @ingroup Traits
     *
     * Extends @c InputSettings with dropdown-specific geometry.
     * Colors live in @c SelectTheme.
     *
     * @see InputSettings, SelectTheme, SelectStyle, Select
     */
    struct SelectSettings : InputSettings
    {
        float        itemHeight      = 40.f;
        float        maxVisibleItems = 6.f;
        unsigned int descCharSize    = 11;

        // ── Assignment from parent levels ─────────────────────────────────────

        SelectSettings& operator=(const InputSettings& i)
        { static_cast<InputSettings&>(*this) = i; return *this; }

        SelectSettings& operator=(const ControlSettings& c)
        { static_cast<InputSettings&>(*this) = c; return *this; }

        SelectSettings& operator=(const GraphicSettings& g)
        { static_cast<InputSettings&>(*this) = g; return *this; }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] float        getItemHeight()      const { return itemHeight; }
        [[nodiscard]] float        getMaxVisibleItems() const { return maxVisibleItems; }
        [[nodiscard]] unsigned int getDescCharSize()    const { return descCharSize; }

        void setItemHeight(float h)         { itemHeight      = h; }
        void setMaxVisibleItems(float n)    { maxVisibleItems = n; }
        void setDescCharSize(unsigned int s){ descCharSize    = s; }
    };

} // namespace ml

#endif // MALENA_SELECTSETTINGS_H
