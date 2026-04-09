//
// Checkbox.h
//

#ifndef CHECKBOX_H
#define CHECKBOX_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Interaction/Selectable.h>
#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Resources/FontManager.h>
#include <Malena/Traits/Settings/CheckboxSettings.h>
#include <Malena/Traits/Theme/CheckboxTheme.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <functional>
#include <string>
#include <type_traits>

namespace ml
{
    class MALENA_API CheckboxManifest : public ml::Manifest
    {
    public:
        enum class Flag  { CHECKED, DISABLED };
        enum class State { IDLE, HOVERED, CHECKED, DISABLED };
    };

    /**
     * @brief A toggleable checkbox with a square indicator and text label.
     * @ingroup GraphicsControls
     *
     * Inherits @c CheckboxSettings (geometry) and @c CheckboxTheme (colors/font)
     * as first-class members. The active theme is applied automatically via
     * @c Themeable. A font passed to the constructor overrides the themed font.
     *
     * @see CheckboxSettings, CheckboxTheme, CheckboxStyle, CheckboxGroup
     */
    class MALENA_API Checkbox : public ComponentWith<CheckboxManifest>,
                     public CheckboxSettings,
                     public CheckboxTheme,
                     public Themeable,
                     public Selectable
    {
    public:
        using Flag  = CheckboxManifest::Flag;
        using State = CheckboxManifest::State;

    private:
        sf::RectangleShape _box;
        sf::VertexArray    _checkmark;
        sf::Text           _label;
        sf::Vector2f       _position = {0.f, 0.f};
        std::string        _labelStr;

        void layout();
        void buildCheckmark();
        void applyVisualState();

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit Checkbox(const std::string& label = "",
                          const sf::Font& font = FontManager<>::getDefault());

        // ── Apply ─────────────────────────────────────────────────────────────

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<CheckboxSettings, S>,
                "applySettings() requires a type derived from CheckboxSettings");
            static_cast<CheckboxSettings&>(*this) = s;
            layout();
        }

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<CheckboxTheme, T>,
                "applyTheme() requires a type derived from CheckboxTheme");
            static_cast<CheckboxTheme&>(*this) = t;
            applyVisualState();
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<CheckboxSettings, St> &&
                          std::is_base_of_v<CheckboxTheme, St>,
                "applyStyle() requires a type derived from both CheckboxSettings and CheckboxTheme");
            static_cast<CheckboxSettings&>(*this) = s;
            static_cast<CheckboxTheme&>(*this)    = s;
            layout();
        }

        // ── Checked state ─────────────────────────────────────────────────────

        void check();
        void uncheck();
        void toggle();
        [[nodiscard]] bool isChecked() const;

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
    class CheckboxWith : public Checkbox, public Customizable<MANIFEST>
    { public: using Checkbox::Checkbox; };

} // namespace ml

#endif // CHECKBOX_H
