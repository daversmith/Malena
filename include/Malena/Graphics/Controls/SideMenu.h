//
// SideMenu.h
//

#ifndef SIDEMENU_H
#define SIDEMENU_H

#pragma once

#include <Malena/Core/Component.h>
#include <Malena/Core/Core.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Resources/FontManager.h>
#include <Malena/Graphics/Controls/List.h>
#include <Malena/Traits/Settings/SideMenuSettings.h>
#include <Malena/Traits/Theme/SideMenuTheme.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <type_traits>

namespace ml
{
    class SideMenuManifest : public ml::Manifest
    {
    public:
        enum class Flag  { OPEN };
        enum class State { CLOSED, OPEN, ANIMATING };
    };

    /**
     * @brief A slide-in panel anchored to the left or right edge of the window.
     * @ingroup GraphicsControls
     *
     * @c SideMenu manages a panel that slides in/out with a built-in hamburger
     * button. It supports two display modes:
     *
     * - **OVERLAY** — slides over the content with a semi-transparent backdrop.
     * - **PUSH** — slides in alongside the content (fires @c onOpen / @c onClose
     *   callbacks so the app can shift its layout accordingly).
     *
     * The panel can host a @c List (via @c getList()) or any @c ml::Core
     * component (via @c setContent()). The @c SideMenu draws and positions the
     * content internally; no separate @c addComponent() call is needed.
     *
     * ### Usage
     * @code
     * ml::SideMenu menu;
     * menu.setPanelWidth(280.f);
     *
     * auto& list = menu.getList();
     * list.addItem("Home");
     * list.addItem("Settings");
     * list.addItem("Profile");
     *
     * addComponent(menu);   // hamburger, backdrop, and panel drawing
     * @endcode
     *
     * ### PUSH mode — shift layout on open
     * @code
     * menu.setMode(ml::SideMenuSettings::Mode::PUSH);
     * menu.onOpen([&] { myContent.setPosition({menu.getPanelWidth(), 0.f}); });
     * menu.onClose([&]{ myContent.setPosition({0.f, 0.f}); });
     * @endcode
     *
     * @see SideMenuSettings, SideMenuTheme, SideMenuStyle, List
     */
    class SideMenu : public ComponentWith<SideMenuManifest>,
                     public SideMenuSettings,
                     public SideMenuTheme,
                     public Themeable
    {
    public:
        using Flag  = SideMenuManifest::Flag;
        using State = SideMenuManifest::State;
        using Mode   = SideMenuSettings::Mode;
        using Anchor = SideMenuSettings::Anchor;

    private:
        // ── Content ───────────────────────────────────────────────────────────
        ml::List  _defaultList;
        ml::Core* _content;        ///< Points to _defaultList unless overridden

        // ── Visual ────────────────────────────────────────────────────────────
        sf::RectangleShape _panel;
        sf::RectangleShape _backdrop;
        sf::Vector2f       _hamburgerPos = {8.f, 8.f};

        // ── Animation ─────────────────────────────────────────────────────────
        float _currentX  = 0.f;
        float _targetX   = 0.f;
        bool  _animating = false;

        // ── Callbacks ─────────────────────────────────────────────────────────
        std::function<void()> _onOpenCb;
        std::function<void()> _onCloseCb;

        // ── Internal ──────────────────────────────────────────────────────────
        float closedX()     const;   ///< Off-screen X for the closed state
        float openX()       const;   ///< On-screen X for the open state
        float resolvedHeight() const;
        void  updatePanelTransform();
        void  drawHamburger(sf::RenderTarget& target,
                            const sf::RenderStates& states) const;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit SideMenu(Mode   mode   = Mode::OVERLAY,
                          Anchor anchor = Anchor::LEFT,
                          const sf::Font& font = FontManager<>::getDefault());

        // ── Apply ─────────────────────────────────────────────────────────────

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<SideMenuSettings, S>,
                "applySettings() requires a type derived from SideMenuSettings");
            static_cast<SideMenuSettings&>(*this) = s;
            updatePanelTransform();
        }

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<SideMenuTheme, T>,
                "applyTheme() requires a type derived from SideMenuTheme");
            static_cast<SideMenuTheme&>(*this) = t;
            _panel.setFillColor(panelBg);
            _backdrop.setFillColor(backdropColor);
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<SideMenuSettings, St> &&
                          std::is_base_of_v<SideMenuTheme, St>,
                "applyStyle() requires SideMenuSettings and SideMenuTheme");
            static_cast<SideMenuSettings&>(*this) = s;
            static_cast<SideMenuTheme&>(*this)    = s;
            _panel.setFillColor(panelBg);
            _backdrop.setFillColor(backdropColor);
            updatePanelTransform();
        }

        // ── Content ───────────────────────────────────────────────────────────

        /**
         * @brief Return the internal @c List for configuration.
         *
         * The @c SideMenu draws and positions this list internally. List items
         * receive events automatically — no separate @c addComponent() call is needed.
         */
        ml::List& getList();

        /**
         * @brief Replace the internal list with any component.
         *
         * The @c SideMenu draws and positions the component internally. The
         * component's events fire automatically — no separate @c addComponent()
         * call is needed.
         */
        void setContent(ml::Core& component);

        // ── Control ───────────────────────────────────────────────────────────

        void open();
        void close();
        void toggle();
        [[nodiscard]] bool isOpen() const;

        // ── Callbacks ─────────────────────────────────────────────────────────

        /** @brief Called when the panel finishes opening. */
        void onOpen(std::function<void()> cb);
        /** @brief Called when the panel finishes closing. */
        void onClose(std::function<void()> cb);

        // ── Sizing / position ─────────────────────────────────────────────────

        void setHamburgerPosition(const sf::Vector2f& pos);
        [[nodiscard]] float getPanelWidth() const { return panelWidth; }

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class SideMenuWith : public SideMenu, public Customizable<MANIFEST>
    { public: using SideMenu::SideMenu; };

} // namespace ml
#endif // SIDEMENU_H
