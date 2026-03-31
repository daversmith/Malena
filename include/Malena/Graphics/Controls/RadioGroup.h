//
// RadioGroup.h
//

#ifndef RADIOGROUP_H
#define RADIOGROUP_H

#pragma once

#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Graphics/Controls/RadioButton.h>
#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Resources/FontManager.h>
#include <Malena/Traits/Settings/RadioGroupSettings.h>
#include <Malena/Traits/Theme/RadioGroupTheme.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>

namespace ml
{
    class RadioGroupManifest : public ml::Manifest
    {
    public:
        enum class Flag { SHOW_BACKGROUND };
    };

    /**
     * @brief A self-contained group of radio buttons with automatic layout.
     * @ingroup GraphicsControls
     *
     * Inherits @c RadioGroupSettings and @c RadioGroupTheme — the inherited
     * fields serve as defaults applied to every item the group creates.
     *
     * @see RadioGroupSettings, RadioGroupTheme, RadioGroupStyle, RadioButton
     */
    class RadioGroup : public ComponentWith<RadioGroupManifest>,
                       public RadioGroupSettings,
                       public RadioGroupTheme,
                       public Themeable
    {
    public:
        using Flag = RadioGroupManifest::Flag;

    private:
        std::vector<std::unique_ptr<RadioButton>> _owned;
        std::vector<RadioButton*>                 _buttons;
        RadioButton*                              _selected = nullptr;

        sf::Vector2f  _position = {0.f, 0.f};
        ml::Rectangle _background;

        std::function<void(const std::string&, std::size_t)> _onSelectionChanged;

        void layout();
        void applyStylesToButton(RadioButton& btn);
        void wire(RadioButton& btn);
        void handleClick(RadioButton& clicked);

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit RadioGroup(const sf::Font& font = FontManager<>::getDefault());

        // ── Apply ─────────────────────────────────────────────────────────────

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<RadioGroupSettings, S>,
                "applySettings() requires a type derived from RadioGroupSettings");
            static_cast<RadioGroupSettings&>(*this) = s;
            for (auto* btn : _buttons) applyStylesToButton(*btn);
            layout();
        }

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<RadioGroupTheme, T>,
                "applyTheme() requires a type derived from RadioGroupTheme");
            static_cast<RadioGroupTheme&>(*this) = t;
            for (auto* btn : _buttons) applyStylesToButton(*btn);
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<RadioGroupSettings, St> &&
                          std::is_base_of_v<RadioGroupTheme, St>,
                "applyStyle() requires RadioGroupSettings and RadioGroupTheme");
            static_cast<RadioGroupSettings&>(*this) = s;
            static_cast<RadioGroupTheme&>(*this)    = s;
            for (auto* btn : _buttons) applyStylesToButton(*btn);
            layout();
        }

        // ── Adding options ────────────────────────────────────────────────────

        void addOption(const std::string& label);
        void add(RadioButton& button);
        void clearOptions();
        [[nodiscard]] std::size_t optionCount() const;

        // ── Selection ─────────────────────────────────────────────────────────

        void select(std::size_t index);
        void selectFirst();
        [[nodiscard]] std::string   getSelectedLabel() const;
        [[nodiscard]] std::size_t   getSelectedIndex() const;
        [[nodiscard]] RadioButton*  getSelected()      const;

        // ── Callback ─────────────────────────────────────────────────────────

        void onSelectionChanged(
            std::function<void(const std::string&, std::size_t)> callback);

        // ── Option enabled ────────────────────────────────────────────────────

        void setOptionEnabled(std::size_t index, bool enabled);

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class RadioGroupWith : public RadioGroup, public Customizable<MANIFEST>
    { public: using RadioGroup::RadioGroup; };

} // namespace ml

#endif // RADIOGROUP_H
