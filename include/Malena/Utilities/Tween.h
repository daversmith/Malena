// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 11/13/22.
//

#ifndef PORTFOLIO_TWEEN_H
#define PORTFOLIO_TWEEN_H

#pragma once

#include <Malena/Core/malena_export.h>

namespace ml
{
    /**
     * @brief Easing curve selection for animated movement.
      * @ingroup Utilities
     *
     * @c Tween is passed to @c Positionable::moveTo and related animation
     * methods to control how intermediate positions are interpolated between
     * the start and end points.
     *
     * | Value | Curve |
     * |-------|-------|
     * | @c LINEAR      | Constant speed from start to end |
     * | @c EXPONENTIAL | Starts fast and decelerates toward the target |
     *
     * ### Usage
     * @code
     * // Animate with default linear easing
     * myComponent.moveTo({400.f, 300.f}, 0.5f);
     *
     * // Animate with exponential ease-out
     * myComponent.moveTo({400.f, 300.f}, 0.5f, ml::EXPONENTIAL);
     * @endcode
     *
     * @note @c LAST_TWEEN is a sentinel value used for bounds checking.
     *       Do not pass it as an actual easing curve.
     *
     * @see Positionable
     */
    enum Tween
    {
        LINEAR,      ///< Constant-speed interpolation.
        EXPONENTIAL, ///< Decelerating interpolation (ease-out).
        LAST_TWEEN   ///< Sentinel — do not use as an actual tween value.
    };

} // namespace ml

#endif // PORTFOLIO_TWEEN_H
