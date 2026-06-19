// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#include <Malena/Resources/IconFont.h>
#include <Malena/Resources/Fonts/materialicons.h>   // big array — included in ONE TU only

namespace ml
{
    const sf::Font& iconFont()
    {
        static sf::Font  font;
        static const bool loaded =
            font.openFromMemory(fonts::materialicons_data.data(),
                                fonts::materialicons_data_size);
        (void)loaded;
        return font;
    }
}
