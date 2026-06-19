// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#ifndef MALENA_DIALOGREQUEST_H
#define MALENA_DIALOGREQUEST_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <functional>
#include <string>

namespace ml
{
    class Core;
    /**
     * @brief Interface for components that want the root shell to present a
     *        confirm/cancel dialog on their behalf.
     * @ingroup Traits
     *
     * Inherit @c DialogRequest alongside @c Hookable and call
     * @c sendHook(ml::DialogRequest::Hook::SHOW) to ask whatever shell is
     * listening to display a modal with your message and labels.
     *
     * The shell subscribes with:
     * @code
     * onHook(ml::DialogRequest::Hook::SHOW, [this](ml::DialogRequest& d) {
     *     _modal.setConfirmLabel(d.getConfirmLabel());
     *     _modal.setCancelLabel(d.getCancelLabel());
     *     _modal.setContent(_text);
     *     _text.setString(d.getMessage());
     *     if (auto cb = d.getOnConfirm()) _modal.onConfirm(std::move(cb));
     *     if (auto cb = d.getOnReject())  _modal.onDismiss(std::move(cb));
     *     _modal.show();
     * });
     * @endcode
     *
     * @see Hookable, Modal
     */
    struct MALENA_API DialogRequest
    {
        enum class Hook { SHOW };

        virtual const std::string&    getMessage()      = 0;
        virtual const std::string&    getConfirmLabel() = 0;
        virtual const std::string&    getCancelLabel()  = 0;
        virtual std::function<void()> getOnConfirm()    = 0;
        virtual std::function<void()> getOnReject()     = 0;

        // Optional overrides — existing subclasses need no changes
        virtual std::string getTitle()   { return {}; }
        virtual ml::Core*   getContent() { return nullptr; }

        virtual ~DialogRequest() = default;
    };

} // namespace ml

#endif // MALENA_DIALOGREQUEST_H
