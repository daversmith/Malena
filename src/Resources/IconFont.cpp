// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

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
