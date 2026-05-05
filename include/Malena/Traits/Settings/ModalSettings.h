#ifndef MALENA_MODALSETTINGS_H
#define MALENA_MODALSETTINGS_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Settings/ControlSettings.h>

namespace ml
{
    struct ModalSettings : ControlSettings
    {
        sf::Vector2f modalSize         = {420.f, 300.f};
        float        cornerRadius      = 12.f;
        float        titleBarHeight    = 50.f;
        float        buttonBarHeight   = 56.f;
        bool         dismissOnBackdrop = true;
        bool         showCloseButton   = true;

        ModalSettings& operator=(const ControlSettings& c)
        { static_cast<ControlSettings&>(*this) = c; return *this; }

        void setModalSize(const sf::Vector2f& s) { modalSize      = s; }
        void setCornerRadius(float r)            { cornerRadius   = r; }
        void setTitleBarHeight(float h)          { titleBarHeight = h; }
        void setButtonBarHeight(float h)         { buttonBarHeight = h; }
        void setDismissOnBackdrop(bool v)        { dismissOnBackdrop = v; }
        void setShowCloseButton(bool v)          { showCloseButton   = v; }
    };

} // namespace ml

#endif // MALENA_MODALSETTINGS_H
