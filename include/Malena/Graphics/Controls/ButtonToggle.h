//
// ButtonToggle.h
//

#ifndef BUTTONTOGGLE_H
#define BUTTONTOGGLE_H

#pragma once

#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Settings/ButtonSettings.h>
#include <Malena/Traits/Themeable.h>
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <string>

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
     *
     * Supports optional icons with configurable placement relative to the label.
     *
     * @code
     * ml::ButtonToggle save;
     * save.setButtonOffLabel("Save");
     * save.setButtonOnLabel("Saved");
     * save.setSize({120.f, 36.f});
     * save.onToggled([](bool on){ });
     * addComponent(save);
     * @endcode
     *
     * @see ButtonSettings, Themeable
     */
    class ButtonToggle : public ComponentWith<ButtonToggleManifest>,
                         public ButtonSettings,
                         public Themeable
    {
    public:
        using Flag  = ButtonToggleManifest::Flag;
        using State = ButtonToggleManifest::State;

    private:
        sf::Vector2f _position = {0.f, 0.f};
        std::function<void(bool)> _onToggled;

        void syncFromSettings();
        void drawCapsule(sf::RenderTarget&, const sf::RenderStates&,
                         sf::Vector2f, sf::Vector2f, float, sf::Color) const;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit ButtonToggle();

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<ButtonSettings, S>,
                "ButtonToggle::applySettings() requires a type derived from ButtonSettings");
            static_cast<ButtonSettings&>(*this) = s;
            syncFromSettings();
        }

        void setOn(bool on);
        void toggle();
        [[nodiscard]] bool isOn() const;

        void setEnabled(bool enabled);
        [[nodiscard]] bool isEnabled() const;

        void setSize(const sf::Vector2f& size);
        [[nodiscard]] sf::Vector2f getSize() const;

        void setButtonOnLabel(const std::string& label);
        void setButtonOffLabel(const std::string& label);
        void setButtonOnColor(const sf::Color& color);
        void setButtonOffColor(const sf::Color& color);
        void setButtonOnTextColor(const sf::Color& color);
        void setButtonOffTextColor(const sf::Color& color);
        void setBorderColor(const sf::Color& color);
        void setBorderThickness(float thickness);
        void setButtonRadius(float radius);

        void setIcon(const sf::Texture* onIcon, const sf::Texture* offIcon = nullptr);
        void setIconSize(const sf::Vector2f& size);
        void setIconAlign(ButtonSettings::IconAlign align);
        void setIconPadding(float padding);

        void setFont(const sf::Font& f);
        void setFont(const sf::Font&&) = delete;
        void setCharacterSize(unsigned int size);

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
