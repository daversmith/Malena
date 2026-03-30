//
// Toggle.h
//

#ifndef TOGGLE_H
#define TOGGLE_H

#pragma once

#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Resources/FontManager.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Clock.hpp>
#include <functional>
#include <string>
#include <vector>

namespace ml
{
    // ── ToggleManifest ────────────────────────────────────────────────────────

    class ToggleManifest : public ml::Manifest
    {
    public:
        enum class Flag  { ON, DISABLED };
        enum class State { IDLE, HOVERED, ON, DISABLED };
    };

    // ── Toggle ────────────────────────────────────────────────────────────────

    /**
     * @brief A versatile on/off toggle switch.
     * @ingroup GraphicsControls
     *
     * Supports three visual styles set via @c setStyle():
     *
     * | Style      | Appearance                                           |
     * |------------|------------------------------------------------------|
     * | @c PILL    | iOS-style oval track with animated sliding thumb     |
     * | @c CHECKBOX| Square that fills with a checkmark when on          |
     * | @c BUTTON  | Rectangular button that changes color and label      |
     *
     * Both a left label and a right label are supported independently.
     * Labels sit flush against the toggle on each side.
     *
     * ### Usage — PILL style
     * @code
     * ml::Toggle darkMode;
     * darkMode.setStyle(ml::Toggle::Style::PILL);
     * darkMode.setRightLabel("Dark mode");
     * darkMode.setPosition({200.f, 100.f});
     * darkMode.onToggled([](bool on){ std::cout << on << "\n"; });
     * addComponent(darkMode);
     * @endcode
     *
     * ### Usage — BUTTON style
     * @code
     * ml::Toggle soundToggle;
     * soundToggle.setStyle(ml::Toggle::Style::BUTTON);
     * soundToggle.setButtonOnLabel("Sound ON");
     * soundToggle.setButtonOffLabel("Sound OFF");
     * soundToggle.setSize({120.f, 36.f});
     * addComponent(soundToggle);
     * @endcode
     *
     * ### Usage — CHECKBOX style
     * @code
     * ml::Toggle acceptTerms;
     * acceptTerms.setStyle(ml::Toggle::Style::CHECKBOX);
     * acceptTerms.setRightLabel("I accept the terms");
     * addComponent(acceptTerms);
     * @endcode
     *
     * @see ToggleGroup, ToggleManifest
     */
    class Toggle : public ComponentWith<ToggleManifest>
    {
    public:
        using Flag  = ToggleManifest::Flag;
        using State = ToggleManifest::State;

        /**
         * @brief Visual presentation style of the toggle.
         */
        enum class Style
        {
            PILL,       ///< iOS-style oval track with sliding circular thumb
            CHECKBOX,   ///< Square box that fills with a check when on
            BUTTON      ///< Rectangular button changing color/label on toggle
        };

    private:
        Style        _style     = Style::PILL;
        sf::Vector2f _position  = {0.f, 0.f};

        // ── PILL ──────────────────────────────────────────────────────────────
        sf::RectangleShape _track;
        sf::CircleShape    _thumb;
        sf::Vector2f       _trackSize     = {44.f, 24.f};
        float              _thumbMargin   = 2.f;
        float              _thumbX        = 0.f;   // lerp value 0=off, 1=on
        float              _thumbTarget   = 0.f;
        float              _animSpeed     = 10.f;
        mutable sf::Clock  _animClock;
        sf::Color          _trackOnColor  = sf::Color(70,  200, 100);
        sf::Color          _trackOffColor = sf::Color(120, 120, 120);
        sf::Color          _thumbColor    = sf::Color::White;

        // ── CHECKBOX ──────────────────────────────────────────────────────────
        sf::RectangleShape _box;
        sf::Vector2f       _boxSize       = {20.f, 20.f};
        float              _boxRadius     = 3.f;
        sf::Color          _boxBorderColor= sf::Color(120, 120, 120);
        sf::Color          _boxOnColor    = sf::Color(70,  130, 230);
        sf::Color          _boxOffColor   = sf::Color(40,  40,  40);
        float              _boxThickness  = 1.5f;

        // ── BUTTON ────────────────────────────────────────────────────────────
        sf::RectangleShape _button;
        sf::Text           _buttonText;
        sf::Vector2f       _buttonSize    = {100.f, 36.f};
        std::string        _buttonOnLabel = "ON";
        std::string        _buttonOffLabel= "OFF";
        sf::Color          _buttonOnColor = sf::Color(70,  130, 230);
        sf::Color          _buttonOffColor= sf::Color(60,  60,  60);
        sf::Color          _buttonOnTextColor = sf::Color::White;
        sf::Color          _buttonOffTextColor= sf::Color(180, 180, 180);
        float              _buttonRadius  = 4.f;
        float              _buttonThick   = 1.5f;
        sf::Color          _buttonBorder  = sf::Color(100, 100, 100);

        // ── Labels ────────────────────────────────────────────────────────────
        sf::Text  _leftLabel;
        sf::Text  _rightLabel;
        float     _labelOffset   = 8.f;
        sf::Color _labelColor    = sf::Color::White;
        sf::Color _labelDisColor = sf::Color(120, 120, 120);

        // ── Disabled ──────────────────────────────────────────────────────────
        sf::Color _disabledTint = sf::Color(120, 120, 120);

        // ── Font ──────────────────────────────────────────────────────────────
        const sf::Font* _font     = &FontManager<>::getDefault();
        unsigned int    _charSize = 14;

        // ── Callback ─────────────────────────────────────────────────────────
        std::function<void(bool)> _onToggled;

        // ── Internal helpers ──────────────────────────────────────────────────
        void layout();
        void applyVisualState();
        void updateThumbPosition();

        /// Bounds of the interactive widget element (track, box, or button)
        sf::FloatRect widgetBounds() const;

        void drawPill(sf::RenderTarget& target, const sf::RenderStates& states) const;
        void drawCheckbox(sf::RenderTarget& target, const sf::RenderStates& states) const;
        void drawButton(sf::RenderTarget& target, const sf::RenderStates& states) const;
        void drawCheckmark(sf::RenderTarget& target, const sf::RenderStates& states) const;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    public:
        explicit Toggle(const sf::Font& font = FontManager<>::getDefault());

        // ── Style ─────────────────────────────────────────────────────────────

        /**
         * @brief Set the visual style. Triggers a full layout rebuild.
         * @param style @c PILL, @c CHECKBOX, or @c BUTTON.
         */
        void setStyle(Style style);
        [[nodiscard]] Style getStyle() const;

        // ── State ─────────────────────────────────────────────────────────────

        void setOn(bool on);
        void toggle();
        [[nodiscard]] bool isOn() const;

        void setEnabled(bool enabled);
        [[nodiscard]] bool isEnabled() const;

        // ── Labels ────────────────────────────────────────────────────────────

        /** @brief Set label displayed to the left of the toggle. */
        void setLeftLabel(const std::string& text);

        /** @brief Set label displayed to the right of the toggle. */
        void setRightLabel(const std::string& text);

        [[nodiscard]] std::string getLeftLabel()  const;
        [[nodiscard]] std::string getRightLabel() const;

        void setLabelColor(const sf::Color& color);
        void setLabelOffset(float offset);

        // ── PILL styling ──────────────────────────────────────────────────────

        void setTrackSize(const sf::Vector2f& size);
        void setThumbMargin(float margin);
        void setTrackOnColor(const sf::Color& color);
        void setTrackOffColor(const sf::Color& color);
        void setThumbColor(const sf::Color& color);
        void setAnimationSpeed(float speed);

        // ── CHECKBOX styling ──────────────────────────────────────────────────

        void setBoxSize(const sf::Vector2f& size);
        void setBoxOnColor(const sf::Color& color);
        void setBoxOffColor(const sf::Color& color);
        void setBoxBorderColor(const sf::Color& color);
        void setBoxBorderThickness(float thickness);
        void setBoxRadius(float radius);

        // ── BUTTON styling ────────────────────────────────────────────────────

        /**
         * @brief Set the size of the button (BUTTON style only).
         * @param size Width and height of the button in pixels.
         */
        void setSize(const sf::Vector2f& size);

        /** @brief Label shown on the button when the toggle is ON. */
        void setButtonOnLabel(const std::string& label);

        /** @brief Label shown on the button when the toggle is OFF. */
        void setButtonOffLabel(const std::string& label);

        void setButtonOnColor(const sf::Color& color);
        void setButtonOffColor(const sf::Color& color);
        void setButtonOnTextColor(const sf::Color& color);
        void setButtonOffTextColor(const sf::Color& color);
        void setButtonBorderColor(const sf::Color& color);
        void setButtonBorderThickness(float thickness);
        void setButtonRadius(float radius);

        // ── Font ──────────────────────────────────────────────────────────────

        void setFont(const sf::Font& font);
        void setFont(const sf::Font&&) = delete;
        void setCharacterSize(unsigned int size);
        [[nodiscard]] unsigned int getCharacterSize() const;

        // ── Callback ─────────────────────────────────────────────────────────

        /**
         * @brief Register a callback invoked when the toggle state changes.
         * @param callback Called with @c true when on, @c false when off.
         */
        void onToggled(std::function<void(bool)> callback);

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    // ── ToggleGroup ───────────────────────────────────────────────────────────

    /**
     * @brief A vertical collection of @c Toggle switches.
     * @ingroup GraphicsControls
     *
     * @c ToggleGroup lays out multiple @c Toggle components vertically with
     * consistent spacing. Each toggle is independent — unlike @c RadioGroup,
     * multiple toggles can be on simultaneously.
     *
     * ### Usage — group creates items
     * @code
     * ml::ToggleGroup settings;
     * settings.setPosition({200.f, 100.f});
     * settings.addToggle("Dark mode",   "dark_mode");
     * settings.addToggle("Subtitles",   "subtitles");
     * settings.addToggle("Fullscreen",  "fullscreen");
     *
     * settings.onAnyToggled([](const std::string& key, bool on){
     *     std::cout << key << ": " << on << "\n";
     * });
     * addComponent(settings);
     * @endcode
     *
     * ### Usage — add existing Toggle
     * @code
     * ml::Toggle myToggle;
     * myToggle.setStyle(ml::Toggle::Style::BUTTON);
     * myToggle.setSize({100.f, 36.f});
     * group.add(myToggle, "my_key");
     * @endcode
     *
     * @see Toggle
     */
    class ToggleGroup : public ComponentWith<ToggleManifest>
    {
    public:
        using Flag  = ToggleManifest::Flag;
        using State = ToggleManifest::State;

    private:
        struct Entry
        {
            Toggle*     toggle;
            std::string key;
            bool        owned; // true if we created it via addToggle
        };

        std::vector<Entry>        _entries;
        std::vector<std::unique_ptr<Toggle>> _owned;

        sf::Vector2f _position    = {0.f, 0.f};
        float        _spacing     = 12.f;
        Toggle::Style _defaultStyle = Toggle::Style::PILL;

        const sf::Font* _font     = &FontManager<>::getDefault();
        unsigned int    _charSize = 14;

        std::function<void(const std::string&, bool)> _onAnyToggled;

        void relayout();
        void attachCallback(Entry& entry);

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    public:
        explicit ToggleGroup(const sf::Font& font = FontManager<>::getDefault());

        // ── Adding toggles ────────────────────────────────────────────────────

        /**
         * @brief Create and add a new toggle with a right label.
         *
         * @param label Display label shown to the right of the toggle.
         * @param key   Logical key returned in @c onAnyToggled callback.
         * @param on    Initial state. Defaults to @c false.
         * @return Reference to the newly created @c Toggle.
         */
        Toggle& addToggle(const std::string& label,
                          const std::string& key,
                          bool               on = false);

        /**
         * @brief Add an externally created @c Toggle to the group.
         *
         * The toggle must outlive the group.
         * Do NOT call @c addComponent on it separately.
         *
         * @param toggle The toggle to add.
         * @param key    Logical key returned in @c onAnyToggled callback.
         */
        void add(Toggle& toggle, const std::string& key);

        /** @brief Remove all toggles from the group. */
        void clearToggles();

        // ── State access ──────────────────────────────────────────────────────

        /** @brief Return @c true if the toggle with the given key is on. */
        [[nodiscard]] bool isOn(const std::string& key) const;

        /** @brief Set the toggle with the given key on or off. */
        void setOn(const std::string& key, bool on);

        /** @brief Set all toggles on. */
        void setAllOn();

        /** @brief Set all toggles off. */
        void setAllOff();

        /** @brief Return the keys of all toggles that are currently on. */
        [[nodiscard]] std::vector<std::string> getOnKeys() const;

        // ── Callbacks ─────────────────────────────────────────────────────────

        /**
         * @brief Register a callback invoked when any toggle changes.
         * @param callback Called with the toggle's key and new state.
         */
        void onAnyToggled(std::function<void(const std::string&, bool)> callback);

        // ── Styling ───────────────────────────────────────────────────────────

        /** @brief Set the default style applied to toggles created via @c addToggle. */
        void setDefaultStyle(Toggle::Style style);

        /** @brief Set the vertical spacing between toggles. */
        void setSpacing(float spacing);

        void setFont(const sf::Font& font);
        void setFont(const sf::Font&&) = delete;
        void setCharacterSize(unsigned int size);

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    // ── ToggleWith ────────────────────────────────────────────────────────────

    template<typename MANIFEST>
    class ToggleWith : public Toggle, public Customizable<MANIFEST>
    {
    public:
        using Toggle::Toggle;
    };

    template<typename MANIFEST>
    class ToggleGroupWith : public ToggleGroup, public Customizable<MANIFEST>
    {
    public:
        using ToggleGroup::ToggleGroup;
    };

} // namespace ml

#endif // TOGGLE_H
