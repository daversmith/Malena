// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#ifndef MALENA_OVERLAYWIDGET_H
#define MALENA_OVERLAYWIDGET_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Core.h>

namespace ml
{
    /**
     * @brief Interface for components that provide a drawable overlay layer.
     * @ingroup Traits
     *
     * Inherit @c OverlayWidget alongside @c Hookable and call
     * @c sendHook(ml::OverlayWidget::Hook::REGISTER) to hand an overlay
     * component to whatever shell is listening. The shell decides when and how
     * to render it.
     *
     * ### Usage
     * @code
     * class MyOverlay : public ml::OverlayWidget, public ml::Hookable { ... };
     *
     * // In shell registerHooks():
     * onHook(ml::OverlayWidget::Hook::REGISTER, [this](ml::OverlayWidget& w) {
     *     _activeOverlay = &w.getOverlay();
     * });
     * @endcode
     *
     * @see SplitPaneWidget, ToolbarWidget
     */
    struct MALENA_API OverlayWidget
    {
        enum class Hook { REGISTER };

        virtual ml::Core& getOverlay() = 0;

        virtual ~OverlayWidget() = default;
    };

} // namespace ml

#endif // MALENA_OVERLAYWIDGET_H
