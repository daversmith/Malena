//
// PillToggle.h
//

#ifndef PILLTOGGLE_H
#define PILLTOGGLE_H

#pragma once

#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Settings/PillSettings.h>
#include <Malena/Traits/Themeable.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Clock.hpp>
#include <functional>
#include <string>

namespace ml
{
    // ── PillToggleManifest ────────────────────────────────────────────────────

    class PillToggleManifest : public ml::Manifest
    {
    public:
        enum class Flag  { ON, DISABLED };
        enum class State { IDLE, HOVERED, ON, DISABLED };
    };

    // ── PillToggle ────────────────────────────────────────────────────────────

    /**
     * @brief iOS-style oval pill switch with animated sliding thumb.
     * @ingroup GraphicsControls
     *
     * @c PillToggle inherits @c PillSettings directly — all settings variables
     * are first-class members of the component. The active theme is applied
     * automatically via @c Themeable. Individual setters update one variable
     * and immediately re-sync the visual state.
     *
     * ### Usage — defaults (reads active theme)
     * @code
     * ml::PillToggle t;
     * t.setRightLabel("Dark mode");
     * t.setPosition({200.f, 100.f});
     * t.onToggled([](bool on){ });
     * addComponent(t);
     * @endcode
     *
     * ### Usage — applySettings (one-stop)
     * @code
     * struct MyPill : ml::PillSettings {
     *     MyPill() {
     *         trackOnColor = sf::Color(70, 200, 100);
     *         trackSize    = {56.f, 28.f};
     *         pillOnLabel  = "ON";
     *         pillOffLabel = "OFF";
     *         rightLabel   = "Notifications";
     *     }
     * };
     * t.applySettings(MyPill{});
     * @endcode
     *
     * ### Usage — from manifest
     * @code
     * t.applySettings(Resources::resource(Settings::MainToggle));
     * @endcode
     *
     * ### Locking from theme/settings changes
     * @code
     * t.lockTheme();     // keeps current colors regardless of theme changes
     * t.lockSettings();  // applySettings() has no effect
     * @endcode
     *
     * @see PillSettings, Themeable, ToggleGroup
     */
    class PillToggle : public ComponentWith<PillToggleManifest>,
                       public PillSettings,
                       public Themeable
    {
    public:
        using Flag  = PillToggleManifest::Flag;
        using State = PillToggleManifest::State;

    private:
        // ── SFML shapes ───────────────────────────────────────────────────────
        // These are purely visual — all style data lives in PillSettings.
        sf::CircleShape _thumb;

        // ── Animation state ───────────────────────────────────────────────────
        float             _thumbX      = 0.f;  ///< current lerp value 0=off 1=on
        float             _thumbTarget = 0.f;  ///< target lerp value
        mutable sf::Clock _animClock;

        // ── World position ────────────────────────────────────────────────────
        sf::Vector2f _position = {0.f, 0.f};

        // ── Callback ─────────────────────────────────────────────────────────
        std::function<void(bool)> _onToggled;

        // ── Internal ──────────────────────────────────────────────────────────

        /// Push all PillSettings values to visual state. Called after any
        /// settings or theme change. This is the ONLY place shapes are styled.
        void syncFromSettings();

        /// Compute and set thumb position from current _thumbX lerp value.
        void updateThumbPosition();

        /// Draw a capsule (pill shape) — used for the track.
        void drawCapsule(sf::RenderTarget&       target,
                         const sf::RenderStates& states,
                         sf::Vector2f            pos,
                         sf::Vector2f            size,
                         float                   radius,
                         sf::Color               fill) const;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        // Themeable — called by ThemeManager when the active theme changes
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit PillToggle();

        // ── applySettings ─────────────────────────────────────────────────────

        /**
         * @brief Apply a settings struct in one call.
         *
         * Copies all settings values onto this component and re-syncs.
         * Always takes effect regardless of lock state — this is an
         * explicit user call, not an automatic update.
         *
         * @tparam S  Must derive from @c PillSettings.
         * @param  s  The settings to apply.
         */
        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<PillSettings, S>,
                "PillToggle::applySettings() requires a type derived from PillSettings");
            static_cast<PillSettings&>(*this) = s;
            syncFromSettings();
        }

        // ── State ─────────────────────────────────────────────────────────────

        void setOn(bool on);
        void toggle();
        [[nodiscard]] bool isOn() const;

        void setEnabled(bool enabled);
        [[nodiscard]] bool isEnabled() const;

        // ── Track styling ─────────────────────────────────────────────────────

        void setTrackSize(const sf::Vector2f& size);
        [[nodiscard]] sf::Vector2f getTrackSize() const;

        void setTrackOnColor(const sf::Color& color);
        void setTrackOffColor(const sf::Color& color);

        // ── Thumb styling ─────────────────────────────────────────────────────

        void setThumbColor(const sf::Color& color);
        void setThumbMargin(float margin);

        // ── Inside labels ─────────────────────────────────────────────────────

        void setInsideLabels(const std::string& offLabel, const std::string& onLabel);
        void setPillLabelColors(const sf::Color& active, const sf::Color& inactive);

        // ── External labels ───────────────────────────────────────────────────

        void setLeftLabel(const std::string& text);
        void setRightLabel(const std::string& text);
        [[nodiscard]] std::string getLeftLabel()  const;
        [[nodiscard]] std::string getRightLabel() const;
        void setLabelColor(const sf::Color& color);
        void setLabelOffset(float offset);

        // ── Animation ────────────────────────────────────────────────────────

        void setAnimationSpeed(float speed);

        // ── Font ──────────────────────────────────────────────────────────────

        void setFont(const sf::Font& f);
        void setFont(const sf::Font&&) = delete;
        void setCharacterSize(unsigned int size);
        [[nodiscard]] unsigned int getCharacterSize() const;

        // ── Callback ─────────────────────────────────────────────────────────

        void onToggled(std::function<void(bool)> callback);

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    // ── PillToggleWith ────────────────────────────────────────────────────────

    template<typename MANIFEST>
    class PillToggleWith : public PillToggle, public Customizable<MANIFEST>
    {
    public:
        using PillToggle::PillToggle;
    };

} // namespace ml

#endif // PILLTOGGLE_H
