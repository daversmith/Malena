//
// SegmentToggle.h
//

#ifndef SEGMENTTOGGLE_H
#define SEGMENTTOGGLE_H

#pragma once

#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Settings/SegmentSettings.h>
#include <Malena/Traits/Themeable.h>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Clock.hpp>
#include <functional>
#include <string>

namespace ml
{
    class SegmentToggleManifest : public ml::Manifest
    {
    public:
        enum class Flag  { ON, DISABLED };
        enum class State { IDLE, HOVERED, ON, DISABLED };
    };

    /**
     * @brief Two-option segmented selector with animated sliding pill.
     * @ingroup GraphicsControls
     *
     * Renders two labelled options side-by-side inside a rounded track.
     * A pill slides behind the selected option with a lerp animation.
     *
     * @code
     * ml::SegmentToggle view;
     * view.setSegmentLabels("Hotels", "Apartments");
     * view.setSize({220.f, 38.f});
     * view.onToggled([](bool on){
     *     std::string choice = on ? "Apartments" : "Hotels";
     * });
     * addComponent(view);
     * @endcode
     *
     * @see SegmentSettings, Themeable
     */
    class SegmentToggle : public ComponentWith<SegmentToggleManifest>,
                          public SegmentSettings,
                          public Themeable
    {
    public:
        using Flag  = SegmentToggleManifest::Flag;
        using State = SegmentToggleManifest::State;

    private:
        sf::Vector2f      _position    = {0.f, 0.f};
        float             _thumbX      = 0.f;
        float             _thumbTarget = 0.f;
        mutable sf::Clock _animClock;

        std::function<void(bool)> _onToggled;

        void syncFromSettings();
        void drawCapsule(sf::RenderTarget&, const sf::RenderStates&,
                         sf::Vector2f, sf::Vector2f, float, sf::Color) const;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit SegmentToggle();

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<SegmentSettings, S>,
                "SegmentToggle::applySettings() requires a type derived from SegmentSettings");
            static_cast<SegmentSettings&>(*this) = s;
            syncFromSettings();
        }

        void setOn(bool on);
        void toggle();
        [[nodiscard]] bool isOn() const;

        void setEnabled(bool enabled);
        [[nodiscard]] bool isEnabled() const;

        void setSegmentLabels(const std::string& offLabel, const std::string& onLabel);
        void setSize(const sf::Vector2f& size);
        [[nodiscard]] sf::Vector2f getSize() const;
        void setTrackColor(const sf::Color& color);
        void setThumbColor(const sf::Color& color);
        void setActiveTextColor(const sf::Color& color);
        void setInactiveTextColor(const sf::Color& color);
        void setSegmentRadius(float radius);
        void setSegmentPadding(float padding);
        void setAnimationSpeed(float speed);
        void setFont(const sf::Font& f);
        void setFont(const sf::Font&&) = delete;
        void setCharacterSize(unsigned int size);

        void onToggled(std::function<void(bool)> callback);

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class SegmentToggleWith : public SegmentToggle, public Customizable<MANIFEST>
    { public: using SegmentToggle::SegmentToggle; };

} // namespace ml

#endif // SEGMENTTOGGLE_H
