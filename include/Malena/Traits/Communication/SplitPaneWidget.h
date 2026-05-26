// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_SPLITPANEWIDGET_H
#define MALENA_SPLITPANEWIDGET_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Graphics/Layouts/Panel.h>

namespace ml
{
    /**
     * @brief Interface for components that fill the left and right sides of a
     *        split-pane shell.
     * @ingroup Traits
     *
     * Inherit @c SplitPaneWidget alongside @c Hookable to participate in any
     * shell that manages a split layout. The shell decides how to present and
     * activate the widget (toolbar button, sidebar list, tab, etc.) — this
     * interface only describes how to populate the panels.
     *
     * ### Usage
     * @code
     * // App-level shell widget with a toolbar label
     * struct GraphicWidget : public ml::SplitPaneWidget, public ml::Hookable {
     *     virtual std::string getToolbarLabel() = 0;
     * };
     *
     * // In registerHooks():
     * onHook(Hook::GRAPHIC, [this](GraphicWidget& w) {
     *     _toolbar.addButton(w.getToolbarLabel(), [this, &w]() {
     *         _left->clear();  _right->clear();
     *         w.fillLeft(*_left);  w.fillRight(*_right);
     *     });
     * });
     * @endcode
     *
     * @see Panel, OverlayWidget, ToolbarWidget
     */
    struct MALENA_API SplitPaneWidget
    {
        virtual ml::Core& getLeftPanel()  = 0;
        virtual ml::Core& getRightPanel() = 0;

        virtual void fillLeft (ml::Panel& panel) { panel.addRef(getLeftPanel());  }
        virtual void fillRight(ml::Panel& panel) { panel.addRef(getRightPanel()); }

        virtual ~SplitPaneWidget() = default;
    };

} // namespace ml

#endif // MALENA_SPLITPANEWIDGET_H
