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
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <string>
#include <vector>
#include <memory>

namespace ml
{
    // ── RadioGroupManifest ────────────────────────────────────────────────────

    /**
     * @brief Manifest for the @c RadioGroup component.
     * @ingroup GraphicsControls
     */
    class RadioGroupManifest : public ml::Manifest
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

    // ── RadioGroup ────────────────────────────────────────────────────────────

    /**
     * @brief A self-contained group of radio buttons with automatic layout
     *        and mutual exclusion.
     * @ingroup GraphicsControls
     *
     * @c RadioGroup is a full @c Component — register it once with
     * @c addComponent and it handles everything internally: layout,
     * mutual exclusion, and drawing all buttons.
     *
     * Options can be added by label string (the group creates the button)
     * or by passing an existing @c RadioButton (user retains ownership of
     * the external reference but the group drives selection).
     *
     * ### Usage — addOption (simplest)
     * @code
     * ml::RadioGroup difficulty;
     * difficulty.addOption("Easy");
     * difficulty.addOption("Medium");
     * difficulty.addOption("Hard");
     * difficulty.setPosition({200.f, 100.f});
     * difficulty.selectFirst();
     *
     * difficulty.onSelectionChanged([](const std::string& label, std::size_t index){
     *     std::cout << "Selected: " << label << "\n";
     * });
     *
     * addComponent(difficulty);
     * @endcode
     *
     * ### Usage — add(RadioButton&) (custom buttons)
     * @code
     * ml::RadioButton easy, medium, hard;
     * easy.setLabel("Easy");
     * medium.setLabel("Medium");
     * hard.setLabel("Hard");
     *
     * ml::RadioGroup group;
     * group.add(easy);
     * group.add(medium);
     * group.add(hard);
     * group.setPosition({200.f, 100.f});
     * group.selectFirst();
     *
     * addComponent(group);  // draws all buttons — do NOT addComponent them separately
     * @endcode
     *
     * ### Optional background
     * @code
     * group.showBackground(true);
     * group.setBackgroundColor(sf::Color(30, 30, 30, 200));
     * group.setBackgroundRadius(8.f);
     * group.setPadding(12.f);
     * @endcode
     *
     * ### Visual customization
     * @code
     * group.setRadius(9.f);
     * group.setDotColor(sf::Color(70, 130, 230));
     * group.setSpacing(24.f);
     * group.setCharacterSize(14);
     * group.setLabelColor(sf::Color::White);
     * @endcode
     *
     * ### With a manifest
     * @code
     * class DifficultyGroup : public ml::RadioGroupWith<DifficultyManifest> {};
     * @endcode
     *
     * @see RadioButton, RadioGroupManifest, Selectable, ComponentWith
     */
    class RadioGroup : public ComponentWith<RadioGroupManifest>
    {
    public:
        /// Convenience alias for @c RadioGroupManifest::Flag.
        using Flag = RadioGroupManifest::Flag;

    private:
        // ── Button storage ────────────────────────────────────────────────────

        /// Buttons created internally via addOption()
        std::vector<std::unique_ptr<RadioButton>> _owned;

        /// All buttons — owned + externally passed via add()
        std::vector<RadioButton*>                 _buttons;

        /// Currently selected button
        RadioButton*                              _selected = nullptr;

        // ── Layout ────────────────────────────────────────────────────────────
        sf::Vector2f _position  = {0.f, 0.f};
        float        _spacing   = 28.f;
        float        _padding   = 0.f;

        // ── Optional background ───────────────────────────────────────────────
        ml::Rectangle _background;
        bool          _showBackground = false;

        // ── Shared visual properties ──────────────────────────────────────────
        float           _radius         = 10.f;
        float           _dotScale       = 0.5f;
        float           _labelOffset    = 8.f;
        float           _ringThickness  = 2.f;
        unsigned int    _charSize       = 16;
        sf::Color       _ringColor      = sf::Color(120, 120, 120);
        sf::Color       _ringHoverColor = sf::Color(80,  80,  80);
        sf::Color       _ringSelColor   = sf::Color(70,  130, 230);
        sf::Color       _dotColor       = sf::Color(70,  130, 230);
        sf::Color       _labelColor     = sf::Color::White;
        const sf::Font* _font           = &FontManager<>::getDefault();

        // ── Callback ──────────────────────────────────────────────────────────
        std::function<void(const std::string&, std::size_t)> _onSelectionChanged;

        // ── Internal helpers ──────────────────────────────────────────────────
        void layout();
        void applyStylesToButton(RadioButton& btn);
        void wire(RadioButton& btn);
        void handleClick(RadioButton& clicked);

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    public:
        /**
         * @brief Construct an empty radio group.
         *
         * @param font Font used for all internally created button labels.
         *             Defaults to the built-in Arial font.
         */
        explicit RadioGroup(const sf::Font& font = FontManager<>::getDefault());

        // ── Adding options ────────────────────────────────────────────────────

        /**
         * @brief Add a new option by label string.
         *
         * Creates a @c RadioButton internally, applies the current visual
         * style, wires it into the selection logic, and re-runs layout.
         * The group owns the button's lifetime.
         *
         * @param label Display label for this option.
         */
        void addOption(const std::string& label);

        /**
         * @brief Add an existing @c RadioButton to the group.
         *
         * The group does not own the button — the caller is responsible for
         * its lifetime. The button must outlive this @c RadioGroup.
         * The group draws the button and drives its selection state.
         * Do NOT call @c addComponent on the button separately.
         *
         * @param button The radio button to add.
         */
        void add(RadioButton& button);

        /**
         * @brief Remove all options from the group.
         *
         * Destroys all internally owned buttons and clears references to
         * externally added buttons. Does not destroy external buttons.
         */
        void clearOptions();

        /**
         * @brief Return the total number of options in the group.
         */
        [[nodiscard]] std::size_t optionCount() const;

        // ── Selection ─────────────────────────────────────────────────────────

        /**
         * @brief Programmatically select an option by zero-based index.
         *
         * Deselects the current selection, selects the option at @p index,
         * and fires @c onSelectionChanged. Has no effect if @p index is out
         * of range or the target button is disabled.
         *
         * @param index Zero-based index of the option to select.
         */
        void select(std::size_t index);

        /**
         * @brief Select the first option in the group.
         *
         * Safe to call on an empty group.
         */
        void selectFirst();

        /**
         * @brief Return the label of the currently selected option.
         *
         * Returns an empty string if nothing is selected.
         */
        [[nodiscard]] std::string getSelectedLabel() const;

        /**
         * @brief Return the zero-based index of the currently selected option.
         *
         * Returns @c std::string::npos if nothing is selected.
         */
        [[nodiscard]] std::size_t getSelectedIndex() const;

        /**
         * @brief Return a pointer to the currently selected button.
         *
         * Returns @c nullptr if nothing is selected.
         */
        [[nodiscard]] RadioButton* getSelected() const;

        /**
         * @brief Register a callback invoked whenever the selection changes.
         *
         * @param callback Function invoked with the label and zero-based index
         *                 of the newly selected option.
         */
        void onSelectionChanged(std::function<void(const std::string&, std::size_t)> callback);

        // ── Enabled / disabled ────────────────────────────────────────────────

        /**
         * @brief Enable or disable a specific option by zero-based index.
         *
         * A disabled option cannot be clicked or selected.
         *
         * @param index   Zero-based index of the option to modify.
         * @param enabled @c true to enable, @c false to disable.
         */
        void setOptionEnabled(std::size_t index, bool enabled);

        // ── Background ────────────────────────────────────────────────────────

        /**
         * @brief Show or hide the optional background rectangle.
         *
         * When visible, the background is sized automatically to fit all
         * buttons plus the current padding.
         *
         * @param show @c true to show the background, @c false to hide it.
         */
        void showBackground(bool show);

        /**
         * @brief Set the background fill color.
         * @param color Fill color of the background rectangle.
         */
        void setBackgroundColor(const sf::Color& color);

        /**
         * @brief Set the background outline color.
         * @param color Outline color of the background rectangle.
         */
        void setBackgroundOutlineColor(const sf::Color& color);

        /**
         * @brief Set the background outline thickness.
         * @param thickness Outline thickness in pixels.
         */
        void setBackgroundOutlineThickness(float thickness);

        /**
         * @brief Set the background corner radius.
         * @param radius Corner radius in pixels. @c 0 for sharp corners.
         */
        void setBackgroundRadius(float radius);

        /**
         * @brief Set the padding between the background edge and the buttons.
         *
         * Only meaningful when @c showBackground(true) is set.
         *
         * @param padding Padding in pixels on all sides. Defaults to @c 0.
         */
        void setPadding(float padding);

        /** @brief Return the current padding in pixels. */
        [[nodiscard]] float getPadding() const;

        // ── Layout ────────────────────────────────────────────────────────────

        /**
         * @brief Set the pixel gap between consecutive buttons.
         * @param spacing Gap in pixels. Defaults to @c 28.
         */
        void  setSpacing(float spacing);

        /** @brief Return the current spacing between buttons in pixels. */
        [[nodiscard]] float getSpacing() const;

        // ── Visual properties (applied to all buttons) ────────────────────────

        /** @brief Set the outer ring radius on all buttons. */
        void setRadius(float radius);

        /** @brief Set the inner dot scale on all buttons. */
        void setDotScale(float scale);

        /** @brief Set the gap between ring and label on all buttons. */
        void setLabelOffset(float offset);

        /** @brief Set the ring outline thickness on all buttons. */
        void setRingThickness(float thickness);

        /** @brief Set the label character size on all buttons. */
        void setCharacterSize(unsigned int size);

        /** @brief Set the idle ring color on all buttons. */
        void setRingColor(const sf::Color& color);

        /** @brief Set the hover ring color on all buttons. */
        void setRingHoverColor(const sf::Color& color);

        /** @brief Set the selected ring color on all buttons. */
        void setRingSelectedColor(const sf::Color& color);

        /** @brief Set the inner dot color on all buttons. */
        void setDotColor(const sf::Color& color);

        /** @brief Set the label text color on all buttons. */
        void setLabelColor(const sf::Color& color);

        /**
         * @brief Set the font used for all button labels.
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

    // ── RadioGroupWith ────────────────────────────────────────────────────────

    /**
     * @brief @c RadioGroup with an attached manifest.
     *
     * @tparam MANIFEST A @c Manifest subclass declaring @c Flag/@c State enums.
     *
     * @see RadioGroup, Customizable
     */
    template<typename MANIFEST>
    class RadioGroupWith : public RadioGroup, public Customizable<MANIFEST>
    {
    public:
        using RadioGroup::RadioGroup;
    };

} // namespace ml

#endif // RADIOGROUP_H
