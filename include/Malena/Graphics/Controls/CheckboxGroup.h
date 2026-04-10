//
// CheckboxGroup.h
//

#ifndef MALENA_CHECKBOXGROUP_H
#define MALENA_CHECKBOXGROUP_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Graphics/Controls/Checkbox.h>
#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Resources/FontManager.h>
#include <Malena/Traits/Settings/CheckboxGroupSettings.h>
#include <Malena/Traits/Theme/CheckboxGroupTheme.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>

namespace ml
{
    class MALENA_API CheckboxGroupManifest : public ml::Manifest
    {
    public:
        enum class Flag { SHOW_BACKGROUND };
    };

    /**
     * @brief A self-contained group of checkboxes with automatic layout.
     * @ingroup GraphicsControls
     *
     * Inherits @c CheckboxGroupSettings and @c CheckboxGroupTheme — the
     * inherited fields serve as defaults applied to every item the group
     * creates. Call @c applySettings(), @c applyTheme(), or @c applyStyle()
     * before adding options to style the whole group at once.
     *
     * @see CheckboxGroupSettings, CheckboxGroupTheme, CheckboxGroupStyle, Checkbox
     */
    class MALENA_API CheckboxGroup : public ComponentWith<CheckboxGroupManifest>,
                          public CheckboxGroupSettings,
                          public CheckboxGroupTheme,
                          public Themeable
    {
    public:
        using Flag = CheckboxGroupManifest::Flag;

    private:
        std::vector<std::unique_ptr<Checkbox>> _owned;
        std::vector<Checkbox*>                 _checkboxes;

        sf::Vector2f  _position = {0.f, 0.f};
        ml::Rectangle _background;

        std::function<void(const std::vector<std::string>&)> _onSelectionChanged;

        void layout();
        void applyStylesToCheckbox(Checkbox& cb);
        void wire(Checkbox& cb);

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit CheckboxGroup(const sf::Font& font = FontManager<>::getDefault());

        CheckboxGroup(const CheckboxGroup&)            = delete;
        CheckboxGroup& operator=(const CheckboxGroup&) = delete;

        // ── Apply ─────────────────────────────────────────────────────────────

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<CheckboxGroupSettings, S>,
                "applySettings() requires a type derived from CheckboxGroupSettings");
            static_cast<CheckboxGroupSettings&>(*this) = s;
            for (auto* cb : _checkboxes) applyStylesToCheckbox(*cb);
            layout();
        }

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<CheckboxGroupTheme, T>,
                "applyTheme() requires a type derived from CheckboxGroupTheme");
            static_cast<CheckboxGroupTheme&>(*this) = t;
            for (auto* cb : _checkboxes) applyStylesToCheckbox(*cb);
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<CheckboxGroupSettings, St> &&
                          std::is_base_of_v<CheckboxGroupTheme, St>,
                "applyStyle() requires CheckboxGroupSettings and CheckboxGroupTheme");
            static_cast<CheckboxGroupSettings&>(*this) = s;
            static_cast<CheckboxGroupTheme&>(*this)    = s;
            for (auto* cb : _checkboxes) applyStylesToCheckbox(*cb);
            layout();
        }

        // ── Adding options ────────────────────────────────────────────────────

        void addOption(const std::string& label, bool checked = false);
        void add(Checkbox& checkbox);
        void clearOptions();
        [[nodiscard]] std::size_t optionCount() const;

        // ── State ─────────────────────────────────────────────────────────────

        [[nodiscard]] bool isChecked(std::size_t index) const;
        [[nodiscard]] std::vector<std::string>  getCheckedLabels()  const;
        [[nodiscard]] std::vector<std::size_t>  getCheckedIndices() const;

        void check(std::size_t index);
        void uncheck(std::size_t index);
        void checkAll();
        void uncheckAll();

        // ── Callback ─────────────────────────────────────────────────────────

        void onSelectionChanged(
            std::function<void(const std::vector<std::string>&)> callback);

        // ── Option enabled ────────────────────────────────────────────────────

        void setOptionEnabled(std::size_t index, bool enabled);

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class CheckboxGroupWith : public CheckboxGroup, public Customizable<MANIFEST>
    { public: using CheckboxGroup::CheckboxGroup; };

} // namespace ml

#endif // MALENA_CHECKBOXGROUP_H
