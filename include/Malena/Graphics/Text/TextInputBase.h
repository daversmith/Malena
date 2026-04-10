//
// TextInputBase.h
//

#ifndef MALENA_TEXTINPUTBASE_H
#define MALENA_TEXTINPUTBASE_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Resources/FontManager.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <string>

namespace ml
{
    // ── TextInputManifest ─────────────────────────────────────────────────────

    /**
     * @brief Shared manifest for @c TextInput and @c TextArea.
     * @ingroup GraphicsText
     */
    class MALENA_API TextInputManifest : public ml::Manifest
    {
    public:
        /**
         * @brief Runtime boolean flags.
         *
         * | Flag       | Effect                                           |
         * |------------|--------------------------------------------------|
         * | @c DISABLED  | Field cannot be interacted with                |
         * | @c READONLY  | Field can be focused but not edited            |
         */
        enum class Flag { DISABLED, READONLY };

        /**
         * @brief Visual interaction states.
         *
         * | State     | Meaning                                  |
         * |-----------|------------------------------------------|
         * | @c IDLE    | Default resting state                   |
         * | @c FOCUSED | Field has keyboard focus                |
         * | @c DISABLED| Field cannot be interacted with         |
         * | @c ERROR   | Field is in an error / invalid state    |
         */
        enum class State { IDLE, FOCUSED, DISABLED, ERROR };
    };

    // ── TextInputBase ─────────────────────────────────────────────────────────

    /**
     * @brief Shared base for @c TextInput and @c TextArea.
     * @ingroup GraphicsText
     *
     * Provides the styling, state management, placeholder logic, and
     * callback infrastructure common to both single-line and multi-line
     * text input controls. Not intended to be used directly.
     *
     * @see TextInput, TextArea, TextInputManifest
     */
    class MALENA_API TextInputBase : public ComponentWith<TextInputManifest>
    {
    public:
        /// Convenience alias for @c TextInputManifest::Flag.
        using Flag  = TextInputManifest::Flag;
        /// Convenience alias for @c TextInputManifest::State.
        using State = TextInputManifest::State;

    protected:
        // ── Background ────────────────────────────────────────────────────────
        sf::RectangleShape _background;
        sf::Vector2f       _size     = {200.f, 36.f};
        sf::Vector2f       _position = {0.f,   0.f};
        float              _padding  = 8.f;

        // ── Colors ────────────────────────────────────────────────────────────
        sf::Color _bgColor           = sf::Color(40,  40,  40);
        sf::Color _bgFocusedColor    = sf::Color(50,  50,  50);
        sf::Color _bgDisabledColor   = sf::Color(30,  30,  30);
        sf::Color _borderColor       = sf::Color(100, 100, 100);
        sf::Color _borderFocusedColor= sf::Color(70,  130, 230);
        sf::Color _borderErrorColor  = sf::Color(220, 70,  70);
        sf::Color _borderDisabledColor=sf::Color(60,  60,  60);
        sf::Color _textColor         = sf::Color::White;
        sf::Color _placeholderColor  = sf::Color(120, 120, 120);
        sf::Color _disabledTextColor = sf::Color(80,  80,  80);
        float     _borderThickness   = 1.5f;

        // ── Placeholder ───────────────────────────────────────────────────────
        sf::Text  _placeholder;
        bool      _showPlaceholder = true;

        // ── Font ──────────────────────────────────────────────────────────────
        const sf::Font* _font     = &FontManager<>::getDefault();
        unsigned int    _charSize = 16;

        // ── Callbacks ─────────────────────────────────────────────────────────
        std::function<void(const std::string&)> _onChange;

        // ── Internal helpers ──────────────────────────────────────────────────
        void applyBackgroundState();
        void updatePlaceholderPosition();

    public:
        /**
         * @brief Construct a text input base with an optional font.
         * @param font Font for placeholder and text. Defaults to built-in Arial.
         */
        explicit TextInputBase(const sf::Font& font = FontManager<>::getDefault());

        // ── Size ──────────────────────────────────────────────────────────────

        /**
         * @brief Set the width and height of the input field.
         * @param size Width and height in pixels.
         */
        virtual void setSize(const sf::Vector2f& size);

        /** @brief Return the current size of the input field. */
        [[nodiscard]] sf::Vector2f getSize() const;

        // ── Placeholder ───────────────────────────────────────────────────────

        /**
         * @brief Set the placeholder text shown when the field is empty.
         * @param text Placeholder string.
         */
        void setPlaceholder(const std::string& text);

        /** @brief Return the current placeholder string. */
        [[nodiscard]] std::string getPlaceholder() const;

        // ── Enabled / disabled / readonly ─────────────────────────────────────

        /**
         * @brief Enable or disable the input field.
         * @param enabled @c true to enable, @c false to disable.
         */
        void setEnabled(bool enabled);

        /** @brief Return @c true if the field is currently enabled. */
        [[nodiscard]] bool isEnabled() const;

        /**
         * @brief Set the field to read-only mode.
         *
         * A read-only field can be focused and text can be selected,
         * but no editing is permitted.
         *
         * @param readonly @c true for read-only, @c false for editable.
         */
        void setReadOnly(bool readonly);

        /** @brief Return @c true if the field is read-only. */
        [[nodiscard]] bool isReadOnly() const;

        // ── Error state ───────────────────────────────────────────────────────

        /**
         * @brief Put the field into or out of the error visual state.
         *
         * The error state renders the border in @c _borderErrorColor.
         * Useful for validation feedback.
         *
         * @param error @c true to show error state, @c false to clear it.
         */
        void setError(bool error);

        /** @brief Return @c true if the field is currently in error state. */
        [[nodiscard]] bool hasError() const;

        // ── Styling ───────────────────────────────────────────────────────────

        /** @brief Set the background fill color in the idle state. */
        void setBackgroundColor(const sf::Color& color);

        /** @brief Set the background fill color when focused. */
        void setBackgroundFocusedColor(const sf::Color& color);

        /** @brief Set the border color in the idle state. */
        void setBorderColor(const sf::Color& color);

        /** @brief Set the border color when focused. */
        void setBorderFocusedColor(const sf::Color& color);

        /** @brief Set the border color in the error state. */
        void setBorderErrorColor(const sf::Color& color);

        /** @brief Set the border outline thickness in pixels. */
        void setBorderThickness(float thickness);

        /** @brief Set the text color. */
        void setTextColor(const sf::Color& color);

        /** @brief Set the placeholder text color. */
        void setPlaceholderColor(const sf::Color& color);

        /** @brief Set the padding between the border and the text. */
        void setPadding(float padding);

        /** @brief Return the current padding in pixels. */
        [[nodiscard]] float getPadding() const;

        // ── Font ──────────────────────────────────────────────────────────────

        /**
         * @brief Set the font used for text and placeholder.
         * @param font Const reference to the font.
         */
        virtual void setFont(const sf::Font& font);

        /// @brief Deleted — rvalue font references are not supported.
        void setFont(const sf::Font&&) = delete;

        /**
         * @brief Set the character size in points.
         * @param size Character size. Defaults to @c 16.
         */
        virtual void setCharacterSize(unsigned int size);

        /** @brief Return the current character size in points. */
        [[nodiscard]] unsigned int getCharacterSize() const;

        // ── Callback ─────────────────────────────────────────────────────────

        /**
         * @brief Register a callback invoked whenever the text content changes.
         * @param callback Function invoked with the current text value.
         */
        void onChange(std::function<void(const std::string&)> callback);

        // ── Positionable overrides ────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition() const override;
        sf::FloatRect getGlobalBounds() const override;
    };

} // namespace ml

#endif // MALENA_TEXTINPUTBASE_H
