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
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <functional>
#include <string>
#include <vector>

namespace ml
{
    // ── RadioButtonManifest ───────────────────────────────────────────────────

    /**
     * @brief Manifest for the @c RadioButton component.
     * @ingroup GraphicsControls
     *
     * Declares the flags and states that drive radio button behavior.
     * These are gathered automatically into the @c RadioButton component's
     * flag and state stores via the @c ComponentWith machinery.
     */
    class RadioButtonManifest : public ml::Manifest
    {
    public:
        /**
         * @brief Runtime boolean flags for the radio button.
         *
         * | Flag       | Effect                                          |
         * |------------|-------------------------------------------------|
         * | @c SELECTED  | Set when this button is the chosen option     |
         * | @c DISABLED  | Set when the button cannot be interacted with |
         */
        enum class Flag  { SELECTED, DISABLED };

        /**
         * @brief Visual interaction states.
         *
         * | State      | Meaning                                        |
         * |------------|------------------------------------------------|
         * | @c IDLE     | Default resting state                         |
         * | @c HOVERED  | Mouse is over the button                      |
         * | @c SELECTED | Button is the currently selected option       |
         * | @c DISABLED | Button cannot be interacted with              |
         */
        enum class State { IDLE, HOVERED, SELECTED, DISABLED };
    };

    // ── RadioButton ───────────────────────────────────────────────────────────

    /**
     * @brief A single radio button option with a circular indicator and text label.
     * @ingroup GraphicsControls
     *
     * @c RadioButton renders a ring shape with an inner dot that appears when
     * selected, and an @c sf::Text label positioned to the right. It inherits
     * @c Selectable, so @c onSelected and @c onDeselected callbacks fire whenever
     * a @c RadioGroup (or user code) calls @c select() or @c deselect().
     *
     * Radio buttons are designed to work inside a @c RadioGroup, which enforces
     * mutual exclusion across a set of options. They can also be used standalone
     * if mutual exclusion is handled manually.
     *
     * ### Usage with RadioGroup (recommended)
     * @code
     * ml::RadioGroup group;
     *
     * ml::RadioButton optA, optB, optC;
     * optA.setLabel("Option A");
     * optB.setLabel("Option B");
     * optC.setLabel("Option C");
     *
     * optA.setPosition({100.f, 100.f});
     * optB.setPosition({100.f, 130.f});
     * optC.setPosition({100.f, 160.f});
     *
     * group.add(optA);
     * group.add(optB);
     * group.add(optC);
     * group.selectFirst();
     *
     * group.onSelectionChanged([](ml::RadioButton& selected){
     *     std::cout << selected.getLabel() << " chosen\n";
     * });
     *
     * addComponent(optA);
     * addComponent(optB);
     * addComponent(optC);
     * @endcode
     *
     * ### Standalone usage
     * @code
     * ml::RadioButton btn;
     * btn.setLabel("Enable feature");
     * btn.setPosition({200.f, 200.f});
     * btn.onSelected([]{ std::cout << "selected\n"; });
     * btn.onDeselected([]{ std::cout << "deselected\n"; });
     * addComponent(btn);
     * @endcode
     *
     * ### Visual customization
     * @code
     * btn.setRadius(10.f);
     * btn.setRingColor(sf::Color(80, 80, 80));
     * btn.setDotColor(sf::Color(70, 130, 230));
     * btn.setRingThickness(2.f);
     * btn.setLabelOffset(8.f);
     * @endcode
     *
     * ### With a manifest
     * @c RadioButtonWith<Manifest> adds custom flag/state support:
     * @code
     * class SettingsButton : public ml::RadioButtonWith<SettingsManifest> {};
     * @endcode
     *
     * @see RadioButtonManifest, RadioGroup, Selectable, ComponentWith
     */
    class RadioButton : public ComponentWith<RadioButtonManifest>,
                        public Selectable
    {
    public:
        /// Convenience alias for @c RadioButtonManifest::Flag.
        using Flag  = RadioButtonManifest::Flag;
        /// Convenience alias for @c RadioButtonManifest::State.
        using State = RadioButtonManifest::State;

    private:
        // ── Visual members ────────────────────────────────────────────────────
        sf::CircleShape _ring;     ///< Outer ring — always visible
        sf::CircleShape _dot;      ///< Inner dot — visible only when selected
        sf::Text        _label;    ///< Text label rendered to the right of the ring

        // ── Layout ────────────────────────────────────────────────────────────
        float        _radius      = 10.f;
        float        _dotScale    = 0.5f;
        float        _labelOffset = 8.f;
        sf::Vector2f _position    = {0.f, 0.f};

        // ── Colors ────────────────────────────────────────────────────────────
        sf::Color _ringColor          = sf::Color(120, 120, 120);
        sf::Color _ringHoverColor     = sf::Color(80,  80,  80);
        sf::Color _ringSelectedColor  = sf::Color(70,  130, 230);
        sf::Color _dotColor           = sf::Color(70,  130, 230);
        sf::Color _ringDisabledColor  = sf::Color(180, 180, 180);
        sf::Color _dotDisabledColor   = sf::Color(200, 200, 200);
        sf::Color _labelColor         = sf::Color::White;
        sf::Color _labelDisabledColor = sf::Color(160, 160, 160);

        // ── Internal helpers ──────────────────────────────────────────────────
        void layout();           ///< Recompute ring, dot, and label positions
        void applyVisualState(); ///< Sync colors and dot visibility to current state

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    public:
        /**
         * @brief Construct a radio button with an optional label and font.
         *
         * @param label Initial label string. Empty by default.
         * @param font  Font for the label. Defaults to built-in Arial.
         */
        explicit RadioButton(const std::string& label = "",
                             const sf::Font& font = FontManager<>::getDefault());

        // ── Selection ─────────────────────────────────────────────────────────

        /**
         * @brief Mark this button as selected.
         *
         * Sets @c Flag::SELECTED, transitions to @c State::SELECTED, shows
         * the inner dot, applies selection colors, then calls
         * @c Selectable::select() to fire any @c onSelected callbacks.
         *
         * Typically called by @c RadioGroup rather than user code directly.
         */
        void select();

        /**
         * @brief Mark this button as deselected.
         *
         * Clears @c Flag::SELECTED, transitions to @c State::IDLE, hides the
         * inner dot, restores idle colors, then calls @c Selectable::deselect()
         * to fire any @c onDeselected callbacks.
         *
         * Typically called by @c RadioGroup rather than user code directly.
         */
        void deselect();

        /**
         * @brief Return @c true if this button is currently selected.
         */
        [[nodiscard]] bool isSelected() const;

        // ── Enabled / disabled ────────────────────────────────────────────────

        /**
         * @brief Enable or disable the radio button.
         *
         * A disabled button ignores click events and cannot be selected.
         * Transitions to @c State::DISABLED and applies disabled colors.
         *
         * @param enabled @c true to enable, @c false to disable.
         */
        void setEnabled(bool enabled);

        /**
         * @brief Return @c true if the button is currently enabled.
         */
        [[nodiscard]] bool isEnabled() const;

        // ── Layout ────────────────────────────────────────────────────────────

        /**
         * @brief Set the outer ring radius in pixels.
         * @param radius Defaults to @c 10.
         */
        void  setRadius(float radius);

        /** @brief Return the outer ring radius in pixels. */
        [[nodiscard]] float getRadius() const;

        /**
         * @brief Set the inner dot size as a fraction of the outer radius.
         *
         * Clamped to [0.1, 0.9]. Defaults to @c 0.5.
         *
         * @param scale Fraction in range [0.1, 0.9].
         */
        void  setDotScale(float scale);

        /** @brief Return the current dot scale fraction. */
        [[nodiscard]] float getDotScale() const;

        /**
         * @brief Set the pixel gap between the ring's right edge and the label.
         * @param offset Gap in pixels. Defaults to @c 8.
         */
        void  setLabelOffset(float offset);

        /** @brief Return the current gap between ring and label in pixels. */
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

        /** @brief Set the ring outline color in the idle state. */
        void setRingColor(const sf::Color& color);

        /** @brief Set the ring outline color when hovered. */
        void setRingHoverColor(const sf::Color& color);

        /** @brief Set the ring outline color when selected. */
        void setRingSelectedColor(const sf::Color& color);

        /** @brief Set the inner dot fill color when selected. */
        void setDotColor(const sf::Color& color);

        /** @brief Set the ring outline thickness in pixels. Defaults to @c 2. */
        void setRingThickness(float thickness);

        /** @brief Set the label text color in the enabled state. */
        void setLabelColor(const sf::Color& color);

        // ── Positionable overrides ────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition() const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    // ── RadioButtonWith ───────────────────────────────────────────────────────

    /**
     * @brief @c RadioButton with an attached manifest.
     *
     * @tparam MANIFEST A @c Manifest subclass declaring @c Flag/@c State enums.
     *
     * @see RadioButton, Customizable
     */
    template<typename MANIFEST>
    class RadioButtonWith : public RadioButton, public Customizable<MANIFEST>
    {
    public:
        using RadioButton::RadioButton;
    };

} // namespace ml

#endif // RADIOBUTTON_H
