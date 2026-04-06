//
// PillStyle.h
//

#ifndef PILLSTYLE_H
#define PILLSTYLE_H

#pragma once

#include <Malena/Traits/Settings/PillSettings.h>
#include <Malena/Traits/Theme/PillTheme.h>

namespace ml
{
    // ── PillStyle ─────────────────────────────────────────────────────────────

    /**
     * @brief Complete style descriptor for @c PillToggle.
     * @ingroup Settings
     *
     * Combines @c PillSettings (layout and behaviour) and @c PillTheme
     * (color tokens) into a single struct. Use when you want to describe a
     * toggle's full appearance in one place — in a manifest, a style sheet,
     * or a design system.
     *
     * ### Usage — one-stop apply
     * @code
     * ml::PillStyle myStyle;
     * myStyle.trackOnColor = sf::Color(70, 200, 100); // from PillTheme
     * myStyle.trackSize    = {56.f, 28.f};            // from PillSettings
     * myStyle.onLabel      = "Active";                // from PillSettings
     *
     * pill.applyStyle(myStyle);
     * @endcode
     *
     * ### Usage — from manifest
     * @code
     * set(Styles::MainToggle, ml::PillStyle{
     *     .trackOnColor = sf::Color(70, 200, 100),
     *     .trackSize    = {56.f, 28.f},
     *     .items        = {{"Dark mode", "dark"}, {"Subtitles", "subs"}}
     * });
     *
     * pill.applyStyle(Resources::resource(Styles::MainToggle));
     * @endcode
     *
     * ### Usage — assign layers independently
     * @code
     * ml::PillStyle style;
     * style = myPillTheme;    // copies theme layer only
     * style = myPillSettings; // copies settings layer only
     * @endcode
     *
     * @see PillSettings, PillTheme, PillToggle
     */
    struct PillStyle : PillSettings, PillTheme
    {
        // ── Assignment from individual layers ─────────────────────────────────

        PillStyle& operator=(const PillSettings& s)
        {
            static_cast<PillSettings&>(*this) = s;
            return *this;
        }

        PillStyle& operator=(const PillTheme& t)
        {
            static_cast<PillTheme&>(*this) = t;
            return *this;
        }

        PillStyle& operator=(const ToggleTheme& t)
        {
            static_cast<PillTheme&>(*this) = t;
            return *this;
        }

        PillStyle& operator=(const ControlTheme& c)
        {
            static_cast<PillTheme&>(*this) = c;
            return *this;
        }

        PillStyle& operator=(const GraphicTheme& g)
        {
            static_cast<PillTheme&>(*this) = g;
            return *this;
        }

        // ── Apply from global theme ───────────────────────────────────────────

        /**
         * @brief Populate all theme-derived fields from the global @c Theme.
         *
         * Settings fields (sizes, labels, animation speed) are not affected.
         */
        PillStyle& applyFrom(const Theme& t)
        {
            PillTheme::applyFrom(t);
            return *this;
        }
    };

    // ── SegmentStyle ──────────────────────────────────────────────────────────

    /**
     * @brief Complete style descriptor for @c SegmentToggle.
     * @ingroup Settings
     * @see SegmentSettings, SegmentTheme, SegmentToggle
     */
    struct SegmentStyle : SegmentSettings, SegmentTheme
    {
        SegmentStyle& operator=(const SegmentSettings& s)
        { static_cast<SegmentSettings&>(*this) = s; return *this; }

        SegmentStyle& operator=(const SegmentTheme& t)
        { static_cast<SegmentTheme&>(*this) = t; return *this; }

        SegmentStyle& operator=(const ToggleTheme& t)
        { static_cast<SegmentTheme&>(*this) = t; return *this; }

        SegmentStyle& operator=(const ControlTheme& c)
        { static_cast<SegmentTheme&>(*this) = c; return *this; }

        SegmentStyle& operator=(const GraphicTheme& g)
        { static_cast<SegmentTheme&>(*this) = g; return *this; }

        SegmentStyle& applyFrom(const Theme& t)
        { SegmentTheme::applyFrom(t); return *this; }
    };

    // ── ButtonStyle ───────────────────────────────────────────────────────────

    /**
     * @brief Complete style descriptor for @c ButtonToggle.
     * @ingroup Settings
     * @see ButtonSettings, ButtonTheme, ButtonToggle
     */
    struct ButtonStyle : ButtonSettings, ButtonTheme
    {
        ButtonStyle& operator=(const ButtonSettings& s)
        { static_cast<ButtonSettings&>(*this) = s; return *this; }

        ButtonStyle& operator=(const ButtonTheme& t)
        { static_cast<ButtonTheme&>(*this) = t; return *this; }

        ButtonStyle& operator=(const ToggleTheme& t)
        { static_cast<ButtonTheme&>(*this) = t; return *this; }

        ButtonStyle& operator=(const ControlTheme& c)
        { static_cast<ButtonTheme&>(*this) = c; return *this; }

        ButtonStyle& operator=(const GraphicTheme& g)
        { static_cast<ButtonTheme&>(*this) = g; return *this; }

        ButtonStyle& applyFrom(const Theme& t)
        { ButtonTheme::applyFrom(t); return *this; }
    };

} // namespace ml

#endif // PILLSTYLE_H
