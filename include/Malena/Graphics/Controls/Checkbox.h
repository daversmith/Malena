//
// Checkbox.h
//

#ifndef CHECKBOX_H
#define CHECKBOX_H

#pragma once

#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Traits/Selectable.h>
#include <Malena/Resources/FontManager.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <string>

namespace ml
{
    // ── CheckboxManifest ──────────────────────────────────────────────────────

    /**
     * @brief Manifest for the @c Checkbox component.
     * @ingroup GraphicsControls
     */
    class CheckboxManifest : public ml::Manifest
    {
    public:
        /**
         * @brief Runtime boolean flags for the checkbox.
         *
         * | Flag      | Effect                                           |
         * |-----------|--------------------------------------------------|
         * | @c CHECKED  | Set when the checkbox is checked               |
         * | @c DISABLED | Set when the checkbox cannot be interacted with|
         */
        enum class Flag { CHECKED, DISABLED };

        /**
         * @brief Visual interaction states.
         *
         * | State      | Meaning                             |
         * |------------|-------------------------------------|
         * | @c IDLE     | Default resting state              |
         * | @c HOVERED  | Mouse is over the checkbox         |
         * | @c CHECKED  | Checkbox is checked                |
         * | @c DISABLED | Checkbox cannot be interacted with |
         */
        enum class State { IDLE, HOVERED, CHECKED, DISABLED };
    };

    // ── Checkbox ──────────────────────────────────────────────────────────────

    /**
     * @brief A toggleable checkbox with a square indicator and text label.
     * @ingroup GraphicsControls
     *
     * @c Checkbox renders a square box with a checkmark when checked, and
     * an @c sf::Text label positioned to the right. Unlike @c RadioButton,
     * a checkbox toggles independently — checking one does not affect others.
     *
     * @c Checkbox inherits @c Selectable, so @c onSelected fires when the
     * checkbox becomes checked and @c onDeselected fires when it is unchecked.
     *
     * ### Usage
     * @code
     * ml::Checkbox rememberMe;
     * rememberMe.setLabel("Remember me");
     * rememberMe.setPosition({200.f, 200.f});
     *
     * rememberMe.onSelected([]{ std::cout << "checked\n"; });
     * rememberMe.onDeselected([]{ std::cout << "unchecked\n"; });
     *
     * addComponent(rememberMe);
     * @endcode
     *
     * ### Visual customization
     * @code
     * box.setBoxSize(16.f);
     * box.setBoxColor(sf::Color(60, 60, 60));
     * box.setCheckColor(sf::Color(70, 130, 230));
     * box.setBoxRadius(3.f);
     * @endcode
     *
     * ### With a manifest
     * @code
     * class SettingsCheck : public ml::CheckboxWith<SettingsManifest> {};
     * @endcode
     *
     * @see CheckboxManifest, CheckboxGroup, Selectable, ComponentWith
     */
    class Checkbox : public ComponentWith<CheckboxManifest>,
                     public Selectable
    {
    public:
        /// Convenience alias for @c CheckboxManifest::Flag.
        using Flag  = CheckboxManifest::Flag;
        /// Convenience alias for @c CheckboxManifest::State.
        using State = CheckboxManifest::State;

    private:
        // ── Visual members ────────────────────────────────────────────────────
        sf::RectangleShape _box;        ///< Square indicator box
        sf::VertexArray    _checkmark;  ///< Checkmark drawn as a line strip
        sf::Text           _label;      ///< Text label to the right of the box

        // ── Layout ────────────────────────────────────────────────────────────
        float        _boxSize     = 16.f;
        float        _boxRadius   = 2.f;
        float        _labelOffset = 8.f;
        sf::Vector2f _position    = {0.f, 0.f};

        // ── Colors ────────────────────────────────────────────────────────────
        sf::Color _boxColor          = sf::Color(60,  60,  60);
        sf::Color _boxHoverColor     = sf::Color(80,  80,  80);
        sf::Color _boxCheckedColor   = sf::Color(70,  130, 230);
        sf::Color _boxDisabledColor  = sf::Color(180, 180, 180);
        sf::Color _checkColor        = sf::Color::White;
        sf::Color _checkDisabledColor= sf::Color(220, 220, 220);
        sf::Color _outlineColor      = sf::Color(120, 120, 120);
        sf::Color _labelColor        = sf::Color::White;
        sf::Color _labelDisabledColor= sf::Color(160, 160, 160);

        // ── Internal helpers ──────────────────────────────────────────────────
        void layout();
        void buildCheckmark();
        void applyVisualState();

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    public:
        /**
         * @brief Construct a checkbox with an optional label and font.
         *
         * @param label Initial label string. Empty by default.
         * @param font  Font for the label. Defaults to built-in Arial.
         */
        explicit Checkbox(const std::string& label = "",
                          const sf::Font& font = FontManager<>::getDefault());

        // ── Checked state ─────────────────────────────────────────────────────

        /**
         * @brief Check this checkbox.
         *
         * Sets @c Flag::CHECKED, transitions to @c State::CHECKED, shows
         * the checkmark, and fires @c Selectable::select() callbacks.
         */
        void check();

        /**
         * @brief Uncheck this checkbox.
         *
         * Clears @c Flag::CHECKED, transitions to @c State::IDLE, hides
         * the checkmark, and fires @c Selectable::deselect() callbacks.
         */
        void uncheck();

        /**
         * @brief Toggle the checkbox between checked and unchecked.
         */
        void toggle();

        /**
         * @brief Return @c true if the checkbox is currently checked.
         */
        [[nodiscard]] bool isChecked() const;

        // ── Enabled / disabled ────────────────────────────────────────────────

        /**
         * @brief Enable or disable the checkbox.
         *
         * A disabled checkbox ignores click events.
         *
         * @param enabled @c true to enable, @c false to disable.
         */
        void setEnabled(bool enabled);

        /**
         * @brief Return @c true if the checkbox is currently enabled.
         */
        [[nodiscard]] bool isEnabled() const;

        // ── Layout ────────────────────────────────────────────────────────────

        /**
         * @brief Set the size of the checkbox square in pixels.
         * @param size Box width and height. Defaults to @c 16.
         */
        void  setBoxSize(float size);

        /** @brief Return the checkbox box size in pixels. */
        [[nodiscard]] float getBoxSize() const;

        /**
         * @brief Set the corner radius of the checkbox box.
         * @param radius Corner radius in pixels. Defaults to @c 2.
         */
        void  setBoxRadius(float radius);

        /** @brief Return the checkbox box corner radius. */
        [[nodiscard]] float getBoxRadius() const;

        /**
         * @brief Set the pixel gap between the box and the label.
         * @param offset Gap in pixels. Defaults to @c 8.
         */
        void  setLabelOffset(float offset);

        /** @brief Return the gap between box and label in pixels. */
        [[nodiscard]] float getLabelOffset() const;

        // ── Label ─────────────────────────────────────────────────────────────

        /**
         * @brief Set the label text.
         * @param label New label string.
         */
        void setLabel(const std::string& label);

        /** @brief Return the current label string. */
        [[nodiscard]] std::string getLabel() const;

        /**
         * @brief Set the label character size in points.
         * @param size Defaults to @c 16.
         */
        void setCharacterSize(unsigned int size);

        /** @brief Return the current label character size in points. */
        [[nodiscard]] unsigned int getCharacterSize() const;

        /**
         * @brief Set the label font.
         * @param font Const reference to the font to use.
         */
        void setFont(const sf::Font& font);

        /// @brief Deleted — rvalue font references are not supported.
        void setFont(const sf::Font&&) = delete;

        // ── Colors ────────────────────────────────────────────────────────────

        /** @brief Set the box fill color in the idle state. */
        void setBoxColor(const sf::Color& color);

        /** @brief Set the box fill color when hovered. */
        void setBoxHoverColor(const sf::Color& color);

        /** @brief Set the box fill color when checked. */
        void setBoxCheckedColor(const sf::Color& color);

        /** @brief Set the box outline color. */
        void setOutlineColor(const sf::Color& color);

        /** @brief Set the checkmark color. */
        void setCheckColor(const sf::Color& color);

        /** @brief Set the label text color. */
        void setLabelColor(const sf::Color& color);

        // ── Positionable overrides ────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition() const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    // ── CheckboxWith ──────────────────────────────────────────────────────────

    /**
     * @brief @c Checkbox with an attached manifest.
     *
     * @tparam MANIFEST A @c Manifest subclass declaring @c Flag/@c State enums.
     *
     * @see Checkbox, Customizable
     */
    template<typename MANIFEST>
    class CheckboxWith : public Checkbox, public Customizable<MANIFEST>
    {
    public:
        using Checkbox::Checkbox;
    };

} // namespace ml

#endif // CHECKBOX_H
