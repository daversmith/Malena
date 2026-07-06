// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#ifndef MALENA_MODAL_H
#define MALENA_MODAL_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Resources/FontManager.h>
#include <Malena/Graphics/Text/Text.h>
#include <Malena/Traits/Settings/ModalSettings.h>
#include <Malena/Traits/Theme/ModalTheme.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <string>

namespace ml
{
    class MALENA_API ModalManifest : public ml::Manifest
    {
    public:
        enum class Flag  { VISIBLE };
        enum class State { HIDDEN, VISIBLE, ANIMATING };
    };

    /**
     * @brief A centered overlay dialog with optional title, content, and action buttons.
     * @ingroup GraphicsControls
     *
     * @c Modal renders a backdrop over the full window and a centered panel with
     * an optional title bar, custom content area, and confirm/cancel buttons.
     *
     * The panel is always centered in the window. All interaction — backdrop
     * dismiss, close button, confirm, cancel — is handled internally and exposed
     * via callbacks.
     *
     * ### Usage
     * @code
     * ml::Modal modal;
     * modal.setTitle("Confirm Action");
     * modal.setConfirmLabel("OK");
     * modal.setCancelLabel("Cancel");
     * modal.onConfirm([this]{ doAction(); });
     * modal.onDismiss([this]{ /* nothing *\/ });
     * addComponent(modal);
     *
     * // Show it in response to a button click:
     * someBtn.onClick([this]{ modal.show(); });
     * @endcode
     *
     * ### Custom content
     * @code
     * ml::TextBox body;
     * body.setString("Are you sure you want to delete this item?");
     * modal.setContent(body);
     * @endcode
     *
     * ### With a manifest
     * @code
     * class MyModal : public ml::ModalWith<MyManifest> {};
     * @endcode
     *
     * @see ModalSettings, ModalTheme, SideMenu
     */
    class MALENA_API Modal : public ComponentWith<ModalManifest>,
                              public ModalSettings,
                              public ModalTheme,
                              public Themeable
    {
    public:
        using Flag  = ModalManifest::Flag;
        using State = ModalManifest::State;

    private:
        // ── Shapes ───────────────────────────────────────────────────────────
        sf::RectangleShape _backdrop;
        sf::RectangleShape _panel;
        sf::RectangleShape _separator;
        sf::RectangleShape _confirmShape;
        sf::RectangleShape _cancelShape;

        // ── Text ─────────────────────────────────────────────────────────────
        ml::Text _titleText;
        ml::Text _confirmText;
        ml::Text _cancelText;
        ml::Text _closeText;

        // ── Labels ────────────────────────────────────────────────────────────
        std::string _titleStr;
        std::string _confirmStr;
        std::string _cancelStr;

        // ── Content ───────────────────────────────────────────────────────────
        ml::Core* _content = nullptr;

        // ── Computed bounds (updated in applyLayout) ──────────────────────────
        sf::FloatRect _panelBounds;
        sf::FloatRect _confirmBounds;
        sf::FloatRect _cancelBounds;
        sf::FloatRect _closeBounds;

        // ── Animation ─────────────────────────────────────────────────────────
        float _alpha     = 0.f;
        bool  _animating = false;
        bool  _fadingIn  = false;
        bool  _prevDown  = false;
        // True when the current hide was triggered by the confirm button, so the
        // fade-out completion fires _onConfirm (not _onDismiss). Firing the callback
        // only once the modal is fully hidden lets a callback open a follow-up dialog
        // on the SAME modal without the fresh show being clobbered by this fade-out.
        bool  _confirmed = false;

        // ── Callbacks ─────────────────────────────────────────────────────────
        std::function<void()> _onConfirm;
        std::function<void()> _onDismiss;

        // ── Internal ──────────────────────────────────────────────────────────
        sf::Vector2f  panelOrigin()  const;
        bool          hasButtons()   const;
        float         contentTop()   const;
        float         contentHeight() const;
        void          applyLayout();
        void          applyAlphaToColor(sf::Color& c, float alpha) const;
        void          drawWithAlpha(sf::RenderTarget& t,
                                    const sf::RenderStates& s) const;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit Modal(const sf::Font& font = FontManager<>::getDefault());

        // ── Content ───────────────────────────────────────────────────────────

        /** @brief Text displayed in the title bar. Pass an empty string to hide it. */
        void setTitle(const std::string& title);

        /** @brief Component drawn in the content area. The Modal positions it. */
        void setContent(ml::Core& content);

        /** @brief Label for the confirm button. Pass empty to hide the button bar. */
        void setConfirmLabel(const std::string& label);

        /** @brief Label for the cancel button. Pass empty to show only the confirm button. */
        void setCancelLabel(const std::string& label);

        // ── Control ───────────────────────────────────────────────────────────

        void show();
        void showImmediate();
        void hide();
        void toggle();
        [[nodiscard]] bool isVisible() const;

        // ── Callbacks ─────────────────────────────────────────────────────────

        /** @brief Fired when the confirm button is clicked. */
        void onConfirm(std::function<void()> cb);

        /** @brief Fired when dismissed — cancel button, close button, or backdrop tap. */
        void onDismiss(std::function<void()> cb);

        // ── Theming ───────────────────────────────────────────────────────────

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<ModalSettings, S>,
                "applySettings() requires a type derived from ModalSettings");
            static_cast<ModalSettings&>(*this) = s;
            applyLayout();
        }

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<ModalTheme, T>,
                "applyTheme() requires a type derived from ModalTheme");
            static_cast<ModalTheme&>(*this) = t;
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<ModalSettings, St> &&
                          std::is_base_of_v<ModalTheme, St>,
                "applyStyle() requires ModalSettings and ModalTheme");
            static_cast<ModalSettings&>(*this) = s;
            static_cast<ModalTheme&>(*this)    = s;
            applyLayout();
        }

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& pos) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    /**
     * @brief @c Modal with an attached manifest.
     * @tparam MANIFEST A @c Manifest subclass declaring @c Flag/@c State enums.
     */
    template<typename MANIFEST>
    class ModalWith : public Modal, public Customizable<MANIFEST>
    { public: using Modal::Modal; };

} // namespace ml

#endif // MALENA_MODAL_H
