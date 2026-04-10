//
// ListItemSettings.h
//

#ifndef MALENA_LISTITEMSETTINGS_H
#define MALENA_LISTITEMSETTINGS_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/ControlSettings.h>

namespace ml
{
    /**
     * @brief Layout and behaviour settings for @c ListItem.
     * @ingroup Traits
     *
     * Colors live in @c ListItemTheme. @c padding comes from
     * @c ControlTheme and is shared across both chains.
     *
     * @see ControlSettings, ListItemTheme, ListItemStyle, ListItem
     */
    struct ListItemSettings : ControlSettings
    {
        float rowHeight = 0.f;  ///< 0 = auto-sized from content
        float slotGap   = 8.f;  ///< Pixel gap between start / content / end slots

        // ── Assignment from parent levels ─────────────────────────────────────

        ListItemSettings& operator=(const ControlSettings& c)
        { static_cast<ControlSettings&>(*this) = c; return *this; }

        ListItemSettings& operator=(const GraphicSettings& g)
        { static_cast<ControlSettings&>(*this) = g; return *this; }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] float getRowHeight() const { return rowHeight; }
        [[nodiscard]] float getSlotGap()   const { return slotGap; }

        void setRowHeight(float h) { rowHeight = h; }
        void setSlotGap(float g)   { slotGap   = g; }
    };

} // namespace ml

#endif // MALENA_LISTITEMSETTINGS_H
