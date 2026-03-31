//
// ButtonToggle.h
//

#ifndef BUTTONTOGGLE_H
#define BUTTONTOGGLE_H

#pragma once

#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Traits/Settings/ButtonSettings.h>
#include <Malena/Traits/Theme/ButtonTheme.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <type_traits>

namespace ml
{
    class ButtonToggleManifest : public ml::Manifest
    {
    public:
        enum class Flag  { ON, DISABLED };
        enum class State { IDLE, HOVERED, ON, DISABLED };
    };

    /**
     * @brief Rectangular button that changes color and label on toggle.
     * @ingroup GraphicsControls
     * @see ButtonSettings, ButtonTheme, ButtonStyle
     */
    class ButtonToggle : public ComponentWith<ButtonToggleManifest>,
                         public ButtonSettings,
                         public ButtonTheme,
                         public Themeable
    {
    public:
        using Flag  = ButtonToggleManifest::Flag;
        using State = ButtonToggleManifest::State;

    private:
        sf::Vector2f _position = {0.f, 0.f};
        std::function<void(bool)> _onToggled;

        void syncFromSettings();

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit ButtonToggle();

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<ButtonSettings, S>,
                "applySettings() requires a type derived from ButtonSettings");
            static_cast<ButtonSettings&>(*this) = s;
            syncFromSettings();
        }

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<ButtonTheme, T>,
                "applyTheme() requires a type derived from ButtonTheme");
            static_cast<ButtonTheme&>(*this) = t;
            syncFromSettings();
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<ButtonSettings, St> &&
                          std::is_base_of_v<ButtonTheme, St>,
                "applyStyle() requires a type derived from both ButtonSettings and ButtonTheme");
            static_cast<ButtonSettings&>(*this) = s;
            static_cast<ButtonTheme&>(*this)    = s;
            syncFromSettings();
        }

        void setOn(bool on);
        void toggle();
        [[nodiscard]] bool isOn()      const;
        void setEnabled(bool enabled);
        [[nodiscard]] bool isEnabled() const;

        void onToggled(std::function<void(bool)> callback);

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class ButtonToggleWith : public ButtonToggle, public Customizable<MANIFEST>
    { public: using ButtonToggle::ButtonToggle; };

} // namespace ml

#endif // BUTTONTOGGLE_H
