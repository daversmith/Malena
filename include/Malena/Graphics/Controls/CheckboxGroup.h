//
// CheckboxGroup.h
//

#ifndef CHECKBOXGROUP_H
#define CHECKBOXGROUP_H

#pragma once

#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Graphics/Controls/Checkbox.h>
#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Resources/FontManager.h>
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <string>
#include <vector>
#include <memory>

namespace ml
{
    // ── CheckboxGroupManifest ─────────────────────────────────────────────────

    /**
     * @brief Manifest for the @c CheckboxGroup component.
     * @ingroup GraphicsControls
     */
    class CheckboxGroupManifest : public ml::Manifest
    {
    public:
        /**
         * @brief Runtime boolean flags for the group.
         *
         * | Flag              | Effect                                        |
         * |-------------------|-----------------------------------------------|
         * | @c SHOW_BACKGROUND| Whether the background rectangle is visible   |
         */
        enum class Flag { SHOW_BACKGROUND };
    };

    // ── CheckboxGroup ─────────────────────────────────────────────────────────

    /**
     * @brief A self-contained group of checkboxes with automatic layout.
     * @ingroup GraphicsControls
     *
     * @c CheckboxGroup is a full @c Component — register it once with
     * @c addComponent. Unlike @c RadioGroup, multiple checkboxes can be
     * checked simultaneously. The group fires @c onSelectionChanged whenever
     * any checkbox is toggled, providing the full set of currently checked
     * labels and indices.
     *
     * Options can be added by label string or by passing an existing
     * @c Checkbox reference.
     *
     * ### Usage — addOption
     * @code
     * ml::CheckboxGroup features;
     * features.addOption("Fullscreen");
     * features.addOption("VSync");
     * features.addOption("Show FPS");
     * features.setPosition({200.f, 100.f});
     *
     * features.onSelectionChanged([](const std::vector<std::string>& checked){
     *     for (const auto& label : checked)
     *         std::cout << label << " is checked\n";
     * });
     *
     * addComponent(features);
     * @endcode
     *
     * ### Usage — add(Checkbox&)
     * @code
     * ml::Checkbox fullscreen, vsync;
     * fullscreen.setLabel("Fullscreen");
     * vsync.setLabel("VSync");
     *
     * ml::CheckboxGroup group;
     * group.add(fullscreen);
     * group.add(vsync);
     * group.setPosition({200.f, 100.f});
     *
     * addComponent(group);  // do NOT addComponent checkboxes separately
     * @endcode
     *
     * ### Optional background
     * @code
     * group.showBackground(true);
     * group.setBackgroundColor(sf::Color(40, 40, 40, 220));
     * group.setBackgroundRadius(8.f);
     * group.setPadding(12.f);
     * @endcode
     *
     * ### With a manifest
     * @code
     * class SettingsGroup : public ml::CheckboxGroupWith<SettingsManifest> {};
     * @endcode
     *
     * @see Checkbox, CheckboxGroupManifest, RadioGroup, ComponentWith
     */
    class CheckboxGroup : public ComponentWith<CheckboxGroupManifest>
    {
    public:
        /// Convenience alias for @c CheckboxGroupManifest::Flag.
        using Flag = CheckboxGroupManifest::Flag;

    private:
        // ── Checkbox storage ──────────────────────────────────────────────────
        std::vector<std::unique_ptr<Checkbox>> _owned;
        std::vector<Checkbox*>                 _checkboxes;

        // ── Layout ────────────────────────────────────────────────────────────
        sf::Vector2f _position = {0.f, 0.f};
        float        _spacing  = 28.f;
        float        _padding  = 0.f;

        // ── Optional background ───────────────────────────────────────────────
        ml::Rectangle _background;
        bool          _showBackground = false;

        // ── Shared visual properties ──────────────────────────────────────────
        float           _boxSize        = 16.f;
        float           _boxRadius      = 2.f;
        float           _labelOffset    = 8.f;
        unsigned int    _charSize       = 16;
        sf::Color       _boxColor       = sf::Color(60,  60,  60);
        sf::Color       _boxHoverColor  = sf::Color(80,  80,  80);
        sf::Color       _boxCheckedColor= sf::Color(70,  130, 230);
        sf::Color       _checkColor     = sf::Color::White;
        sf::Color       _outlineColor   = sf::Color(120, 120, 120);
        sf::Color       _labelColor     = sf::Color::White;
        const sf::Font* _font           = &FontManager<>::getDefault();

        // ── Callback ──────────────────────────────────────────────────────────
        std::function<void(const std::vector<std::string>&)>      _onSelectionChanged;

        // ── Internal helpers ──────────────────────────────────────────────────
        void layout();
        void applyStylesToCheckbox(Checkbox& cb);
        void wire(Checkbox& cb);

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    public:
        /**
         * @brief Construct an empty checkbox group.
         *
         * @param font Font used for all internally created checkbox labels.
         *             Defaults to the built-in Arial font.
         */
        explicit CheckboxGroup(const sf::Font& font = FontManager<>::getDefault());

        // ── Adding options ────────────────────────────────────────────────────

        /**
         * @brief Add a new option by label string.
         *
         * Creates a @c Checkbox internally, applies the current visual
         * style, wires it into the change callback, and re-runs layout.
         *
         * @param label      Display label for this option.
         * @param checked    Initial checked state. Defaults to @c false.
         */
        void addOption(const std::string& label, bool checked = false);

        /**
         * @brief Add an existing @c Checkbox to the group.
         *
         * The group does not own the checkbox. The caller is responsible
         * for its lifetime. Do NOT call @c addComponent on it separately.
         *
         * @param checkbox The checkbox to add.
         */
        void add(Checkbox& checkbox);

        /**
         * @brief Remove all options from the group.
         */
        void clearOptions();

        /**
         * @brief Return the total number of options in the group.
         */
        [[nodiscard]] std::size_t optionCount() const;

        // ── Querying state ────────────────────────────────────────────────────

        /**
         * @brief Return @c true if the checkbox at @p index is checked.
         *
         * Returns @c false if @p index is out of range.
         *
         * @param index Zero-based index.
         */
        [[nodiscard]] bool isChecked(std::size_t index) const;

        /**
         * @brief Return the labels of all currently checked options.
         */
        [[nodiscard]] std::vector<std::string> getCheckedLabels() const;

        /**
         * @brief Return the zero-based indices of all currently checked options.
         */
        [[nodiscard]] std::vector<std::size_t> getCheckedIndices() const;

        // ── Programmatic control ──────────────────────────────────────────────

        /**
         * @brief Check the option at @p index.
         * Has no effect if @p index is out of range or the checkbox is disabled.
         *
         * @param index Zero-based index.
         */
        void check(std::size_t index);

        /**
         * @brief Uncheck the option at @p index.
         * Has no effect if @p index is out of range.
         *
         * @param index Zero-based index.
         */
        void uncheck(std::size_t index);

        /**
         * @brief Check all options in the group.
         */
        void checkAll();

        /**
         * @brief Uncheck all options in the group.
         */
        void uncheckAll();

        // ── Callback ─────────────────────────────────────────────────────────

        /**
         * @brief Register a callback invoked whenever any checkbox is toggled.
         *
         * The callback receives the labels of all currently checked options.
         *
         * @param callback Function invoked with checked labels on any toggle.
         */
        void onSelectionChanged(std::function<void(const std::vector<std::string>&)> callback);

        // ── Enabled / disabled ────────────────────────────────────────────────

        /**
         * @brief Enable or disable a specific option by zero-based index.
         *
         * @param index   Zero-based index.
         * @param enabled @c true to enable, @c false to disable.
         */
        void setOptionEnabled(std::size_t index, bool enabled);

        // ── Background ────────────────────────────────────────────────────────

        /** @brief Show or hide the optional background rectangle. */
        void showBackground(bool show);

        /** @brief Set the background fill color. */
        void setBackgroundColor(const sf::Color& color);

        /** @brief Set the background outline color. */
        void setBackgroundOutlineColor(const sf::Color& color);

        /** @brief Set the background outline thickness. */
        void setBackgroundOutlineThickness(float thickness);

        /** @brief Set the background corner radius. */
        void setBackgroundRadius(float radius);

        /**
         * @brief Set the padding between the background edge and the checkboxes.
         * @param padding Padding in pixels on all sides. Defaults to @c 0.
         */
        void setPadding(float padding);

        /** @brief Return the current padding in pixels. */
        [[nodiscard]] float getPadding() const;

        // ── Layout ────────────────────────────────────────────────────────────

        /**
         * @brief Set the pixel gap between consecutive checkboxes.
         * @param spacing Gap in pixels. Defaults to @c 28.
         */
        void  setSpacing(float spacing);

        /** @brief Return the current spacing between checkboxes in pixels. */
        [[nodiscard]] float getSpacing() const;

        // ── Visual properties (applied to all checkboxes) ─────────────────────

        /** @brief Set the box size on all checkboxes. */
        void setBoxSize(float size);

        /** @brief Set the box corner radius on all checkboxes. */
        void setBoxRadius(float radius);

        /** @brief Set the label offset on all checkboxes. */
        void setLabelOffset(float offset);

        /** @brief Set the label character size on all checkboxes. */
        void setCharacterSize(unsigned int size);

        /** @brief Set the idle box color on all checkboxes. */
        void setBoxColor(const sf::Color& color);

        /** @brief Set the hover box color on all checkboxes. */
        void setBoxHoverColor(const sf::Color& color);

        /** @brief Set the checked box color on all checkboxes. */
        void setBoxCheckedColor(const sf::Color& color);

        /** @brief Set the checkmark color on all checkboxes. */
        void setCheckColor(const sf::Color& color);

        /** @brief Set the box outline color on all checkboxes. */
        void setOutlineColor(const sf::Color& color);

        /** @brief Set the label text color on all checkboxes. */
        void setLabelColor(const sf::Color& color);

        /**
         * @brief Set the font used for all checkbox labels.
         * @param font Const reference to the font to use.
         */
        void setFont(const sf::Font& font);

        /// @brief Deleted — rvalue font references are not supported.
        void setFont(const sf::Font&&) = delete;

        // ── Positionable overrides ────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition() const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    // ── CheckboxGroupWith ─────────────────────────────────────────────────────

    /**
     * @brief @c CheckboxGroup with an attached manifest.
     *
     * @tparam MANIFEST A @c Manifest subclass declaring @c Flag/@c State enums.
     *
     * @see CheckboxGroup, Customizable
     */
    template<typename MANIFEST>
    class CheckboxGroupWith : public CheckboxGroup, public Customizable<MANIFEST>
    {
    public:
        using CheckboxGroup::CheckboxGroup;
    };

} // namespace ml

#endif // CHECKBOXGROUP_H
