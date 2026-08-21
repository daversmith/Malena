// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#ifndef MALENA_ICONFONT_H
#define MALENA_ICONFONT_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <SFML/Graphics/Font.hpp>
#include <cstdint>

namespace ml
{
    /**
     * @brief The bundled Material Icons font (Apache-2.0). Use with @c Icon
     *        codepoints to render crisp UI glyphs.
     *
     * @code
     * sf::Text t(ml::iconFont(), sf::String(static_cast<char32_t>(ml::Icon::Save)), 18);
     * @endcode
     */
    MALENA_API const sf::Font& iconFont();

    /** Common Material Icons codepoints (private-use area). */
    namespace Icon
    {
        inline constexpr char32_t Check       = 0xe5ca; // done / confirm
        inline constexpr char32_t Save        = 0xe161; // floppy disk
        inline constexpr char32_t Delete      = 0xe872; // trash
        inline constexpr char32_t Close        = 0xe5cd; // x
        inline constexpr char32_t Add         = 0xe145; // +
        inline constexpr char32_t FormatBold       = 0xe238;
        inline constexpr char32_t FormatItalic     = 0xe23f;
        inline constexpr char32_t FormatUnderlined = 0xe249;
        inline constexpr char32_t FormatColorText  = 0xe247; // "A" with color bar
        inline constexpr char32_t Palette          = 0xe40a;
        inline constexpr char32_t FormatAlignLeft   = 0xe236;
        inline constexpr char32_t FormatAlignCenter = 0xe234;
        inline constexpr char32_t FormatAlignRight  = 0xe237;
        inline constexpr char32_t FormatListBulleted = 0xe241;
        inline constexpr char32_t FormatListNumbered = 0xe242;
        inline constexpr char32_t Menu              = 0xe5d2; // hamburger ☰
    }
}

#endif // MALENA_ICONFONT_H
