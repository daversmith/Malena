//
// GraphicSettings.h
//

#ifndef GRAPHICSETTINGS_H
#define GRAPHICSETTINGS_H

#pragma once

namespace ml
{
    /**
     * @brief Base settings trait for all visual components — geometry only.
     * @ingroup Traits
     *
     * Root of the settings inheritance chain. Colors and visual tokens
     * live in @c GraphicTheme. @c GraphicSettings exists for the chain
     * and as a future home for geometry-only properties.
     *
     * ### Inheritance chain
     * @code
     * GraphicSettings
     *     └── ControlSettings
     *             └── ToggleSettings
     *                     └── PillSettings
     *                     └── SegmentSettings
     *                     └── ButtonSettings
     * @endcode
     *
     * @see GraphicTheme, ControlSettings
     */
    struct GraphicSettings
    {
        virtual ~GraphicSettings() = default;
    };

} // namespace ml

#endif // GRAPHICSETTINGS_H
