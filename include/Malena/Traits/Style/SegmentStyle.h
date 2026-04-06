//
// SegmentStyle.h
//

#ifndef SEGMENTSTYLE_H
#define SEGMENTSTYLE_H

#pragma once

#include <Malena/Traits/Settings/SegmentSettings.h>
#include <Malena/Traits/Theme/SegmentTheme.h>

namespace ml
{
    /**
     * @brief Complete style descriptor for @c SegmentToggle.
     * @ingroup Traits
     *
     * Combines @c SegmentSettings and @c SegmentTheme.
     *
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

        SegmentStyle& operator=(const ToggleSettings& t)
        { static_cast<SegmentSettings&>(*this) = t; return *this; }

        SegmentStyle& operator=(const ControlSettings& c)
        { static_cast<SegmentSettings&>(*this) = c; return *this; }

        SegmentStyle& applyFrom(const Theme& t)
        { SegmentTheme::applyFrom(t); return *this; }
    };

} // namespace ml

#endif // SEGMENTSTYLE_H
