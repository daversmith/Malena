// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#ifndef MALENA_TOAST_H
#define MALENA_TOAST_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Component.h>
#include <Malena/Graphics/Text/Text.h>
#include <Malena/Graphics/Controls/RectangleButton.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <functional>
#include <string>

namespace ml
{
    /**
     * @brief A transient, dismissible notification anchored to an edge of the window.
     * @ingroup GraphicsControls
     *
     * A @c Toast shows a short message with an optional action button (e.g.
     * "Connect") and a dismiss button. It is hidden by default; call @c show() to
     * reveal it and @c hide() to remove it. While hidden it draws nothing and its
     * buttons are disabled so they never intercept clicks.
     *
     * Unlike @c Modal, a Toast does not block the rest of the UI — it sits in a
     * corner and the user can ignore it. Typical use is a non-intrusive prompt:
     * @code
     * ml::Toast toast;
     * toast.setMessage("Prof. Smith's hub is online");
     * toast.setActionLabel("Connect");
     * toast.onAction ([this]{ connectToHub(); });
     * toast.onDismiss([]{ /* user ignored it *\/ });
     * addComponent(toast);          // add to the app so it overlays the active scene
     * toast.show();
     * @endcode
     *
     * @see Modal
     */
    class MALENA_API Toast : public Component<>
    {
    public:
        enum class Anchor { TOP, BOTTOM };

    private:
        sf::RectangleShape  _bg;
        ml::Text            _message;
        ml::RectangleButton _action;
        ml::RectangleButton _dismiss;

        bool   _visible   = false;
        bool   _hasAction = false;
        Anchor _anchor    = Anchor::BOTTOM;

        std::function<void()> _onAction;
        std::function<void()> _onDismiss;

        void layout();

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    public:
        Toast();

        void setMessage(const std::string& text);
        void setActionLabel(const std::string& label);   // empty hides the action button
        void setAnchor(Anchor a) { _anchor = a; }

        void show();
        void hide();
        [[nodiscard]] bool isVisible() const { return _visible; }

        void onAction (std::function<void()> cb) { _onAction  = std::move(cb); }
        void onDismiss(std::function<void()> cb) { _onDismiss = std::move(cb); }

        void          setPosition(const sf::Vector2f& pos) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

} // namespace ml

#endif // MALENA_TOAST_H
