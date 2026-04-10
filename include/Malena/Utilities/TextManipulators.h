// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave R. Smith on 3/12/25.
//

#ifndef MALENA_TEXTMANIPULATORS_H
#define MALENA_TEXTMANIPULATORS_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <string>
#include <iostream>
#include <sstream>
#include <SFML/Graphics/Text.hpp>

namespace ml
{
    /**
     * @brief Static helpers for text layout and string manipulation.
      * @ingroup Utilities
     *
     * @c TextManipulators provides the word-wrapping logic used internally
     * by @c ml::Text when @c setWordWrap(true) is enabled. Both overloads
     * insert newline characters into the string so that no rendered line
     * exceeds a given pixel width.
     *
     * ### Usage
     * @code
     * // From an existing sf::Text (uses its current font and character size)
     * std::string wrapped = ml::TextManipulators::wordwrap(myText, 300.f);
     * myText.setString(wrapped);
     *
     * // From raw string + explicit font settings
     * std::string wrapped = ml::TextManipulators::wordwrap(
     *     "A long string...", font, 18, 300.f);
     * @endcode
     *
     * @see Text
     */
    class MALENA_API TextManipulators
    {
    public:
        /**
         * @brief Wrap a string to fit within a pixel width, using an existing @c sf::Text.
         *
         * Reads the font and character size from @p text, then inserts newlines
         * at word boundaries so that no line exceeds @p maxwidth pixels when
         * rendered with those settings.
         *
         * @param text     An @c sf::Text whose font and character size define
         *                 the measurement context. The text's current string is
         *                 used as the input.
         * @param maxwidth Maximum line width in pixels.
         * @return A new string with newlines inserted at appropriate word boundaries.
         */
        static std::string wordwrap(const sf::Text& text, float maxwidth);

        /**
         * @brief Wrap a string to fit within a pixel width, given explicit font settings.
         *
         * Measures each word using @p font at @p characterSize and inserts
         * newlines so that no line exceeds @p maxWidth pixels.
         *
         * @param input         The string to wrap.
         * @param font          Font used to measure glyph widths.
         * @param characterSize Character size in points used for measurement.
         * @param maxWidth      Maximum line width in pixels.
         * @return A new string with newlines inserted at appropriate word boundaries.
         */
        static std::string wordwrap(const std::string& input,
                                    const sf::Font& font,
                                    unsigned int characterSize,
                                    float maxWidth);
    };

} // namespace ml

#endif // MALENA_TEXTMANIPULATORS_H
