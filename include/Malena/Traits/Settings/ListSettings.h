//
// ListSettings.h
//

#ifndef LISTSETTINGS_H
#define LISTSETTINGS_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/ControlSettings.h>

namespace ml
{
    /**
     * @brief Layout and behaviour settings for @c List.
     * @ingroup Traits
     *
     * Colors live in @c ListTheme.
     *
     * @see ControlSettings, ListTheme, ListStyle, List
     */
    struct ListSettings : ControlSettings
    {
        bool  showDividers      = true;
        bool  showBackground    = false;
        float dividerThickness  = 1.f;
        float indent            = 20.f;  ///< Indentation added to nested Lists

        // ── Assignment from parent levels ─────────────────────────────────────

        ListSettings& operator=(const ControlSettings& c)
        { static_cast<ControlSettings&>(*this) = c; return *this; }

        ListSettings& operator=(const GraphicSettings& g)
        { static_cast<ControlSettings&>(*this) = g; return *this; }

        // ── Getters / setters ─────────────────────────────────────────────────

        [[nodiscard]] bool  getShowDividers()     const { return showDividers; }
        [[nodiscard]] bool  getShowBackground()   const { return showBackground; }
        [[nodiscard]] float getDividerThickness() const { return dividerThickness; }
        [[nodiscard]] float getIndent()           const { return indent; }

        void setShowDividers(bool b)         { showDividers     = b; }
        void setShowBackground(bool b)       { showBackground   = b; }
        void setDividerThickness(float t)    { dividerThickness = t; }
        void setIndent(float i)              { indent           = i; }
    };

} // namespace ml

#endif // LISTSETTINGS_H
