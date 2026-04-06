//
// ControlSettings.h
//

#ifndef CONTROLSETTINGS_H
#define CONTROLSETTINGS_H

#pragma once

#include <Malena/Traits/Settings/GraphicSettings.h>

namespace ml
{
    /**
     * @brief Settings trait for interactive UI controls — layout only.
     * @ingroup Traits
     *
     * Colors, fonts and spacing live in @c ControlTheme.
     *
     * @see GraphicSettings, ControlTheme, ToggleSettings
     */
    struct ControlSettings : GraphicSettings
    {
        // ── Assignment from parent ────────────────────────────────────────────

        ControlSettings& operator=(const GraphicSettings&)
        {
            return *this;
        }
    };

} // namespace ml

#endif // CONTROLSETTINGS_H
