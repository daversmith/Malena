//
// PillToggle.h
//

#ifndef PILLTOGGLE_H
#define PILLTOGGLE_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Traits/Settings/PillSettings.h>
#include <Malena/Traits/Theme/PillTheme.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Clock.hpp>
#include <functional>
#include <type_traits>

namespace ml
{
    class MALENA_API PillToggleManifest : public ml::Manifest
    {
    public:
        enum class Flag  { ON, DISABLED };
        enum class State { IDLE, HOVERED, ON, DISABLED };
    };

    /**
     * @brief iOS-style oval pill switch with animated sliding thumb.
     * @ingroup GraphicsControls
     *
     * Inherits @c PillSettings (layout/behaviour) and @c PillTheme (colors)
     * as first-class members. The active theme is applied automatically via
     * @c Themeable — only the @c PillTheme layer is updated on theme change,
     * leaving geometry and labels untouched.
     *
     * ### Defaults — reads active theme automatically
     * @code
     * ml::PillToggle t;
     * t.setRightLabel("Dark mode");
     * t.setPosition({200.f, 100.f});
     * t.onToggled([](bool on){ });
     * addComponent(t);
     * @endcode
     *
     * ### Apply settings only (geometry/labels)
     * @code
     * t.applySettings(MyPillSettings{});
     * @endcode
     *
     * ### Apply theme only (colors/font)
     * @code
     * t.applyTheme(MyPillTheme{});
     * @endcode
     *
     * ### Apply both at once
     * @code
     * t.applyStyle(MyPillStyle{});
     * @endcode
     *
     * @see PillSettings, PillTheme, PillStyle, Themeable, ToggleGroup
     */
    class MALENA_API PillToggle : public ComponentWith<PillToggleManifest>,
                       public PillSettings,
                       public PillTheme,
                       public Themeable
    {
    public:
        using Flag  = PillToggleManifest::Flag;
        using State = PillToggleManifest::State;

    private:
        sf::CircleShape   _thumb;
        float             _thumbX      = 0.f;
        float             _thumbTarget = 0.f;
        mutable sf::Clock _animClock;
        sf::Vector2f      _position    = {0.f, 0.f};

        std::function<void(bool)> _onToggled;

        void syncFromSettings();
        void updateThumbPosition();
        void drawCapsule(sf::RenderTarget&, const sf::RenderStates&,
                         sf::Vector2f, sf::Vector2f, float, sf::Color) const;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit PillToggle();

        // ── Apply ─────────────────────────────────────────────────────────────

        /** @brief Apply layout/behaviour settings. Theme layer is unaffected. */
        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<PillSettings, S>,
                "applySettings() requires a type derived from PillSettings");
            static_cast<PillSettings&>(*this) = s;
            syncFromSettings();
        }

        /** @brief Apply color/font theme. Settings layer is unaffected. */
        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<PillTheme, T>,
                "applyTheme() requires a type derived from PillTheme");
            static_cast<PillTheme&>(*this) = t;
            syncFromSettings();
        }

        /** @brief Apply settings and theme in one call. */
        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<PillSettings, St> &&
                          std::is_base_of_v<PillTheme, St>,
                "applyStyle() requires a type derived from both PillSettings and PillTheme");
            static_cast<PillSettings&>(*this) = s;
            static_cast<PillTheme&>(*this)    = s;
            syncFromSettings();
        }

        // ── State ─────────────────────────────────────────────────────────────

        void setOn(bool on);
        void toggle();
        [[nodiscard]] bool isOn()      const;
        void setEnabled(bool enabled);
        [[nodiscard]] bool isEnabled() const;

        // ── Convenience ───────────────────────────────────────────────────────

        /** @brief Alias for @c setFontSize() — matches SFML naming convention. */
        void setCharacterSize(unsigned int size) { setFontSize(size); }

        /** @brief Alias for @c getFontSize() — matches SFML naming convention. */
        [[nodiscard]] unsigned int getCharacterSize() const { return getFontSize(); }

        // ── Callback ─────────────────────────────────────────────────────────

        void onToggled(std::function<void(bool)> callback);

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class PillToggleWith : public PillToggle, public Customizable<MANIFEST>
    { public: using PillToggle::PillToggle; };

} // namespace ml

#endif // PILLTOGGLE_H