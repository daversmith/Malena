//
// RadioButton.h
//

#ifndef RADIOBUTTON_H
#define RADIOBUTTON_H

#pragma once

#include <Malena/Traits/Interaction/Selectable.h>
#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Resources/FontManager.h>
#include <Malena/Traits/Settings/RadioButtonSettings.h>
#include <Malena/Traits/Theme/RadioButtonTheme.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <string>
#include <type_traits>

namespace ml
{
    class RadioButtonManifest : public ml::Manifest
    {
    public:
        enum class Flag  { SELECTED, DISABLED };
        enum class State { IDLE, HOVERED, SELECTED, DISABLED };
    };

    /**
     * @brief A single radio button option with circular indicator and label.
     * @ingroup GraphicsControls
     *
     * Inherits @c RadioButtonSettings (geometry) and @c RadioButtonTheme
     * (colors/font) as first-class members.
     *
     * @see RadioButtonSettings, RadioButtonTheme, RadioButtonStyle, RadioGroup
     */
    class RadioButton : public ComponentWith<RadioButtonManifest>,
                        public RadioButtonSettings,
                        public RadioButtonTheme,
                        public Themeable,
                        public Selectable
    {
    public:
        using Flag  = RadioButtonManifest::Flag;
        using State = RadioButtonManifest::State;

    private:
        sf::CircleShape _ring;
        sf::CircleShape _dot;
        sf::Text        _label;
        sf::Vector2f    _position = {0.f, 0.f};
        std::string     _labelStr;

        void layout();
        void applyVisualState();

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit RadioButton(const std::string& label = "",
                             const sf::Font& font = FontManager<>::getDefault());

        // ── Apply ─────────────────────────────────────────────────────────────

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<RadioButtonSettings, S>,
                "applySettings() requires a type derived from RadioButtonSettings");
            static_cast<RadioButtonSettings&>(*this) = s;
            layout();
        }

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<RadioButtonTheme, T>,
                "applyTheme() requires a type derived from RadioButtonTheme");
            static_cast<RadioButtonTheme&>(*this) = t;
            applyVisualState();
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<RadioButtonSettings, St> &&
                          std::is_base_of_v<RadioButtonTheme, St>,
                "applyStyle() requires RadioButtonSettings and RadioButtonTheme");
            static_cast<RadioButtonSettings&>(*this) = s;
            static_cast<RadioButtonTheme&>(*this)    = s;
            layout();
        }

        // ── Selection ─────────────────────────────────────────────────────────

        void select();
        void deselect();
        [[nodiscard]] bool isSelected() const;

        // ── Enabled / disabled ────────────────────────────────────────────────

        void setEnabled(bool enabled);
        [[nodiscard]] bool isEnabled() const;

        // ── Label ─────────────────────────────────────────────────────────────

        void setLabel(const std::string& label);
        [[nodiscard]] std::string getLabel() const;

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class RadioButtonWith : public RadioButton, public Customizable<MANIFEST>
    { public: using RadioButton::RadioButton; };

} // namespace ml

#endif // RADIOBUTTON_H
