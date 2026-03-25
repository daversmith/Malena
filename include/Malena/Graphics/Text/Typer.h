// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef TYPER_H
#define TYPER_H

#pragma once

#include <Malena/Resources/FontManager.h>
#include <Malena/Graphics/Base/Graphic.h>

namespace ml
{
    /**
     * @brief An animated text object that accepts keyboard input.
      * @ingroup GraphicsText
     *
     * @c Typer extends @c Graphic<sf::Text> with keyboard event handling,
     * making it the editable text layer inside @c TextInput. It registers
     * event subscriptions internally in @c registerEvents() so that key
     * presses and text-entered events update the displayed string.
     *
     * @c Typer is not typically used standalone — @c TextInput composes
     * a @c Typer (the editable content) and a @c Cursor (the blinking
     * caret) into a ready-to-use input field.
     *
     * ### Direct use (unusual)
     * @code
     * ml::Typer typer;
     * typer.setPosition({100.f, 200.f});
     * typer.setCharacterSize(20);
     * addComponent(typer);
     * @endcode
     *
     * @see TextInput, Cursor, Text
     */
    class Typer : public virtual Graphic<sf::Text>
    {
        void registerEvents();

    public:
        using Graphic::Graphic;

        /**
         * @brief Construct a @c Typer with a given font.
         *
         * Calls @c registerEvents() to set up keyboard and text-entered
         * subscriptions.
         *
         * @param font Font for the editable text. Defaults to the built-in
         *             Arial font via @c FontManager<>::getDefault().
         */
        Typer(const sf::Font& font = FontManager<>::getDefault());
    };

} // namespace ml

#endif // TYPER_H
