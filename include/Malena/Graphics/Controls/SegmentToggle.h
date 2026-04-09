//
// SegmentToggle.h
//

#ifndef SEGMENTTOGGLE_H
#define SEGMENTTOGGLE_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Traits/Settings/SegmentSettings.h>
#include <Malena/Traits/Theme/SegmentTheme.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Clock.hpp>
#include <functional>
#include <type_traits>

namespace ml
{
    class MALENA_API SegmentToggleManifest : public ml::Manifest
    {
    public:
        enum class Flag  { ON, DISABLED };
        enum class State { IDLE, HOVERED, ON, DISABLED };
    };

    /**
     * @brief Two-option segmented selector with animated sliding pill.
     * @ingroup GraphicsControls
     *
     * Inherits @c SegmentSettings (geometry/labels) and @c SegmentTheme (colors)
     * as first-class members. The active theme is applied automatically via
     * @c Themeable.
     *
     * ### Usage
     * @code
     * ml::SegmentToggle toggle;
     * toggle.setOffLabel("Light");
     * toggle.setOnLabel("Dark");
     * toggle.setPosition({200.f, 100.f});
     * toggle.onToggled([](bool on){ applyTheme(on); });
     * addComponent(toggle);
     * @endcode
     *
     * @see SegmentSettings, SegmentTheme, SegmentStyle
     */
    class MALENA_API SegmentToggle : public ComponentWith<SegmentToggleManifest>,
                          public SegmentSettings,
                          public SegmentTheme,
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
                "applySettings() requires a type derived from SegmentSettings");
            static_cast<SegmentSettings&>(*this) = s;
            syncFromSettings();
        }

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<SegmentTheme, T>,
                "applyTheme() requires a type derived from SegmentTheme");
            static_cast<SegmentTheme&>(*this) = t;
            syncFromSettings();
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<SegmentSettings, St> &&
                          std::is_base_of_v<SegmentTheme, St>,
                "applyStyle() requires a type derived from both SegmentSettings and SegmentTheme");
            static_cast<SegmentSettings&>(*this) = s;
            static_cast<SegmentTheme&>(*this)    = s;
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
    class SegmentToggleWith : public SegmentToggle, public Customizable<MANIFEST>
    { public: using SegmentToggle::SegmentToggle; };

} // namespace ml

#endif // SEGMENTTOGGLE_H
