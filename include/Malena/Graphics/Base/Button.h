// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef BUTTON_H
#define BUTTON_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <SFML/Graphics/Text.hpp>
#include <Malena/Core/Core.h>
#include <Malena/Resources/FontManager.h>
#include <string>
#include <optional>
namespace ml
{
    /**
     * @brief Generic button template that combines a shape with a text label.
     *
     * @c Button<T, S> layers an @c sf::Text label on top of any framework
     * shape type @c T, exposing a unified API for positioning, styling, and
     * interacting with both the background shape and its label together.
     *
     * @c T must derive from @c ml::Core — this is enforced at compile time
     * via @c static_assert. @c S is the size type that @c T's constructor
     * accepts (e.g., @c sf::Vector2f for @c Rectangle, @c float for @c Circle,
     * @c std::size_t for @c Convex).
     *
     * Click and hover subscriptions are inherited from @c T (which inherits
     * @c ComponentBase and therefore @c Subscribable). The label text is
     * automatically centered over the shape on every @c setPosition call.
     *
     * ### Typical use via concrete subclasses
     * Most code uses the pre-built button types rather than @c Button directly:
     * @code
     * ml::RectangleButton btn;
     * btn.setSize({200.f, 50.f});
     * btn.setPosition({100.f, 200.f});
     * btn.setString("Play");
     * btn.setTextColor(sf::Color::White);
     * btn.onClick([]{ startGame(); });
     * addComponent(btn);
     * @endcode
     *
     * ### With a manifest
     * @code
     * ml::RectangleButtonWith<MyManifest> btn;
     * @endcode
     *
     * @tparam T Shape type — must derive from @c ml::Core.
     *           Typically @c ml::Rectangle, @c ml::Circle, or @c ml::Convex.
     * @tparam S Size parameter type accepted by @c T's constructor.
     *
     * @ingroup GraphicsBase
     * @see RectangleButton, CircleButton, ConvexButton, Graphic
     */
    template<typename T, typename S>
    class Button : public T
    {
        static_assert(std::is_base_of_v<Core, T>,
            "Button<T, S>: T must be derived from ml::Core.");

        sf::Text _text;

    public:
        /**
         * @brief Construct a button with an optional size, label, and font.
         *
         * @param font       Font for the label text. Defaults to the built-in
         *                   Arial font via @c FontManager<>::getDefault().
         * @param buttonSize Optional initial size passed to @c T's constructor.
         *                   If @c std::nullopt, the shape is default-constructed.
         * @param text       Initial label string. Empty by default.
         * @param charSize   Label character size in points. Defaults to 30.
         */
        Button(const sf::Font& font = FontManager<>::getDefault(),
               std::optional<S> buttonSize = std::nullopt,
               const std::string& text = "",
               unsigned int charSize = 30);

        /**
         * @brief Set the world-space position of both the shape and the label.
         *
         * Repositions the background shape and re-centers the text label.
         *
         * @param position New position in world coordinates.
         */
        void setPosition(const sf::Vector2f& position) override;

        // ── Label text ───────────────────────────────────────────────────────

        /**
         * @brief Set the label string.
         * @param text New label text.
         */
        void setString(const sf::String& text);

        /**
         * @brief Return the current label string.
         * @return Const reference to the current @c sf::String.
         */
        [[nodiscard]] const sf::String& getString() const;

        /**
         * @brief Set the label character size.
         * @param size Character size in points.
         */
        void setCharacterSize(unsigned int size);

        /**
         * @brief Return the current label character size.
         * @return Character size in points.
         */
        [[nodiscard]] unsigned int getCharacterSize() const;

        /**
         * @brief Set the label font.
         *
         * @param font Const reference to the font to use.
         */
        void setFont(const sf::Font& font);

        /// @brief Deleted — rvalue font references are not supported.
        void setFont(const sf::Font&& font) = delete;

        /**
         * @brief Return the current label font.
         * @return Const reference to the active @c sf::Font.
         */
        [[nodiscard]] const sf::Font& getFont() const;

        // ── Label styling ─────────────────────────────────────────────────────

        /**
         * @brief Set the label fill color.
         * @param color New text fill color.
         */
        void setTextColor(const sf::Color& color);

        /**
         * @brief Return the current label fill color.
         * @return Current text @c sf::Color.
         */
        [[nodiscard]] sf::Color getTextColor() const;

        /**
         * @brief Set the label outline color.
         * @param color New text outline color.
         */
        void setTextOutlineColor(const sf::Color& color);

        /**
         * @brief Return the current label outline color.
         * @return Current text outline @c sf::Color.
         */
        [[nodiscard]] sf::Color getTextOutlineColor() const;

        /**
         * @brief Set the label outline thickness in pixels.
         * @param thickness Outline thickness. Pass 0 to remove the outline.
         */
        void setTextOutlineThickness(float thickness);

        /**
         * @brief Return the current label outline thickness.
         * @return Outline thickness in pixels.
         */
        [[nodiscard]] float getTextOutlineThickness() const;

        /**
         * @brief Set the label letter spacing factor.
         * @param spacingFactor Multiplier applied to the default letter spacing.
         */
        void setLetterSpacing(float spacingFactor);

        /**
         * @brief Return the current label letter spacing factor.
         * @return Letter spacing multiplier.
         */
        [[nodiscard]] float getLetterSpacing() const;

        /**
         * @brief Set the label line spacing factor.
         * @param spacingFactor Multiplier applied to the default line height.
         */
        void setLineSpacing(float spacingFactor);

        /**
         * @brief Return the current label line spacing factor.
         * @return Line spacing multiplier.
         */
        [[nodiscard]] float getLineSpacing() const;

        /**
         * @brief Set the label text style (bold, italic, underline, etc.).
         * @param style Bitmask of @c sf::Text::Style flags.
         */
        void setStyle(std::uint32_t style);

        /**
         * @brief Return the current label text style bitmask.
         * @return @c sf::Text::Style bitmask.
         */
        [[nodiscard]] std::uint32_t getStyle() const;

        /**
         * @brief Return the world-space position of a character in the label.
         * @param index Zero-based character index.
         * @return World-space position of the character's origin.
         */
        [[nodiscard]] sf::Vector2f findCharacterPos(std::size_t index) const;

        // ── Transform ────────────────────────────────────────────────────────

        /** @brief Scale both shape and label. */
        void setScale(sf::Vector2f scale);

        /** @brief Apply a multiplicative scale to both shape and label. */
        void scale(sf::Vector2f scale);

        /** @brief Translate both shape and label by @p offset. */
        void move(sf::Vector2f offset);

        /** @brief Rotate both shape and label by @p angle. */
        void rotate(sf::Angle angle);

        /** @brief Set the absolute rotation of both shape and label. */
        void setRotation(sf::Angle angle);

        // ── Draw ─────────────────────────────────────────────────────────────

        /**
         * @brief Draw the button shape and its label to a render target.
         *
         * Draws @c T first, then the centered @c sf::Text label on top.
         *
         * @param target SFML render target.
         * @param states Current render states.
         */
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        // ── Forwarded from T ─────────────────────────────────────────────────
        using T::getPosition;
        using T::setFillColor;
        using T::setOutlineColor;
        using T::setOutlineThickness;
        using T::getGlobalBounds;
        using T::getLocalBounds;
        using T::getOrigin;
        using T::getTransform;
        using T::getInverseTransform;
        using T::setOrigin;
        using T::getGeometricCenter;
        using T::getPoint;
        using T::getPointCount;
    };

} // namespace ml

#include "../../../../src/Graphics/Base/Button.tpp"
#endif // BUTTON_H
