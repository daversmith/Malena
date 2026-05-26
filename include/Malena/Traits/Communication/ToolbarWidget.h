// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_TOOLBARWIDGET_H
#define MALENA_TOOLBARWIDGET_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <functional>
#include <string>

namespace ml
{
    /**
     * @brief Interface for components that register a button+action in a toolbar.
     * @ingroup Traits
     *
     * Inherit @c ToolbarWidget alongside @c Hookable and call
     * @c sendHook(ml::ToolbarWidget::Hook::REGISTER) to add a labelled action
     * button to whatever shell is listening.
     *
     * ### Usage
     * @code
     * class MyAction : public ml::ToolbarWidget, public ml::Hookable {
     *     std::string           getToolbarLabel()  override { return "Export"; }
     *     std::function<void()> getToolbarAction() override { return [this]{ doExport(); }; }
     * };
     *
     * // In shell registerHooks():
     * onHook(ml::ToolbarWidget::Hook::REGISTER, [this](ml::ToolbarWidget& w) {
     *     _toolbar.addButton(w.getToolbarLabel(), w.getToolbarAction());
     * });
     * @endcode
     *
     * @see SplitPaneWidget, OverlayWidget
     */
    struct MALENA_API ToolbarWidget
    {
        enum class Hook { REGISTER };

        virtual std::string           getToolbarLabel()  = 0;
        virtual std::function<void()> getToolbarAction() = 0;

        virtual ~ToolbarWidget() = default;
    };

} // namespace ml

#endif // MALENA_TOOLBARWIDGET_H
