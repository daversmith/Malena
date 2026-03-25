// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef TEXT_H
#define TEXT_H

#pragma once

#include <Malena/Resources/FontManager.h>
#include <Malena/Graphics/Base/Graphic.h>
#include <Malena/Traits/Base/Customizable.h>

namespace ml
{
    /**
     * @brief A framework-integrated text object with word-wrap support.
      * @ingroup GraphicsText
     *
     * @c Text wraps @c sf::Text inside @c Graphic<>, giving it the full
     * framework trait set: @c Subscribable (click, hover, update),
     * @c Flaggable, @c Positionable, and @c Draggable.
     *
     * It extends the base @c sf::Text API with two layout helpers:
     * - @c setMaxWidth — constrains text layout to a maximum pixel width
     * - @c setWordWrap — enables automatic line breaking at word boundaries
     *
     * ### Usage
     * @code
     * ml::Text label;
     * label.setString("Hello, Malena!");
     * label.setCharacterSize(24);
     * label.setFillColor(sf::Color::White);
     * label.setPosition({50.f, 100.f});
     * addComponent(label);
     *
     * // With word wrap
     * ml::Text paragraph;
     * paragraph.setMaxWidth(300.f);
     * paragraph.setWordWrap(true);
     * paragraph.setString("A longer string that will wrap at 300 pixels.");
     * @endcode
     *
     * ### With a manifest
     * @c TextWith<Manifest> layers a manifest's flags and states onto
     * the text object:
     * @code
     * class MyLabel : public ml::TextWith<MyManifest> {};
     * @endcode
     *
     * @note The manifest alias is currently named @c TextWidth — this appears
     *       to be a typo for @c TextWith, consistent with the @c SpriteWidth
     *       naming issue. Use @c TextWidth until corrected upstream.
     *
     * @see Graphic, Cursor, Typer, TextInput
     */
    class Text : public virtual Graphic<sf::Text>
    {
    public:
        using Graphic::Graphic;

        /**
         * @brief Construct a @c Text object with a given font.
         *
         * @param font Font to use for rendering. Defaults to the built-in
         *             Arial font via @c FontManager<>::getDefault().
         */
        explicit Text(const sf::Font& font = FontManager<>::getDefault());

        /**
         * @brief Set the displayed string, applying word-wrap if enabled.
         *
         * Overrides @c sf::Text::setString to re-apply word-wrap layout
         * whenever the string changes and @c wordWrap is @c true.
         *
         * @param text The new string to display.
         */
        void setString(const sf::String& text);

        /**
         * @brief Enable or disable automatic word wrapping.
         *
         * When enabled, @c setString inserts line breaks at word boundaries
         * so that no line exceeds @c maxWidth pixels. Has no effect if
         * @c maxWidth is @c 0.
         *
         * @param word_wrap @c true to enable word wrap, @c false to disable.
         */
        void setWordWrap(bool word_wrap);

        /**
         * @brief Set the maximum layout width for word wrapping.
         *
         * Lines longer than @p max_width pixels will be broken at the
         * nearest word boundary when @c wordWrap is @c true.
         * Pass @c 0 to disable the width constraint.
         *
         * @param max_width Maximum line width in pixels.
         */
        void setMaxWidth(float max_width);

    private:
        bool  wordWrap = false;
        float maxWidth = 0.f;
    };

    /**
     * @brief @c Text with an attached manifest.
     *
     * @note Named @c TextWidth — likely a typo for @c TextWith.
     *       Use @c TextWidth until corrected upstream.
     *
     * @tparam Manifest A @c Manifest subclass declaring @c Flag/@c State enums.
     *
     * @see Text, Customizable
     */
    template<typename Manifest>
    class TextWidth : public Text, public Customizable<Manifest> {}; // TODO: rename to TextWith

} // namespace ml

#endif // TEXT_H
