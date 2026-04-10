//
// PillStyle.h
//

#ifndef MALENA_PILLSTYLE_H
#define MALENA_PILLSTYLE_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/PillSettings.h>
#include <Malena/Traits/Theme/PillTheme.h>

namespace ml
{
    /**
     * @brief Complete style descriptor for @c PillToggle.
     * @ingroup Traits
     *
     * Combines @c PillSettings (layout/behaviour) and @c PillTheme (colors)
     * into one struct. Use when you want to describe a toggle's full
     * appearance in one place — in a manifest, a factory, or a design system.
     *
     * The Settings/Theme split is preserved: @c ThemeManager only ever
     * touches the @c PillTheme layer. Geometry and label fields are
     * unaffected by theme changes.
     *
     * ### Usage — manifest
     * @code
     * set(Styles::MainToggle, ml::PillStyle{
     *     .trackOnColor = sf::Color(70, 200, 100),  // PillTheme
     *     .trackSize    = {56.f, 28.f},             // PillSettings
     * });
     * pill.applyStyle(Resources::resource(Styles::MainToggle));
     * @endcode
     *
     * ### Usage — inline struct
     * @code
     * struct GreenPill : ml::PillStyle {
     *     GreenPill() {
     *         trackOnColor = sf::Color(70, 200, 100);
     *         setTrackSize({56.f, 28.f});
     *         setPillLabels("OFF", "ON");
     *     }
     * };
     * pill.applyStyle(GreenPill{});
     * @endcode
     *
     * ### Usage — assign individual layers
     * @code
     * ml::PillStyle style;
     * style = myPillTheme;    // theme layer only
     * style = myPillSettings; // settings layer only
     * @endcode
     *
     * @see PillSettings, PillTheme, PillToggle
     */
    struct PillStyle : PillSettings, PillTheme
    {
        // ── Assignment from individual layers ─────────────────────────────────

        PillStyle& operator=(const PillSettings& s)
        { static_cast<PillSettings&>(*this) = s; return *this; }

        PillStyle& operator=(const PillTheme& t)
        { static_cast<PillTheme&>(*this) = t; return *this; }

        PillStyle& operator=(const ToggleTheme& t)
        { static_cast<PillTheme&>(*this) = t; return *this; }

        PillStyle& operator=(const ControlTheme& c)
        { static_cast<PillTheme&>(*this) = c; return *this; }

        PillStyle& operator=(const GraphicTheme& g)
        { static_cast<PillTheme&>(*this) = g; return *this; }

        PillStyle& operator=(const ToggleSettings& t)
        { static_cast<PillSettings&>(*this) = t; return *this; }

        PillStyle& operator=(const ControlSettings& c)
        { static_cast<PillSettings&>(*this) = c; return *this; }

        // ── Apply from global theme (theme layer only) ────────────────────────

        PillStyle& applyFrom(const Theme& t)
        { PillTheme::applyFrom(t); return *this; }
    };

} // namespace ml

#endif // MALENA_PILLSTYLE_H
