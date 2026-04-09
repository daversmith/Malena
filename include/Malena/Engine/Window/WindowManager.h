// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 3/7/25.
//

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <SFML/Graphics.hpp>

namespace ml::WindowManager
{
    /**
     * @brief Return the framework's shared @c sf::RenderWindow.
      * @ingroup EngineWindow
     *
     * @c WindowManager provides a single, lazily-created @c sf::RenderWindow
     * that is shared across the entire Malena framework. Because the window
     * is stored in a function-local static, it is created on the first call
     * and lives for the duration of the program — no explicit construction
     * or passing of a window pointer is needed.
     *
     * @c AppManager, @c DrawableWrapper, and any other framework class that
     * needs the window reference call this function rather than holding their
     * own pointer, ensuring there is always exactly one window in a Malena
     * application.
     *
     * ### Usage
     * @code
     * // Retrieve the shared window for drawing
     * sf::RenderWindow& window = ml::WindowManager::getWindow();
     * window.draw(myShape);
     *
     * // Pass it explicitly where an sf::RenderWindow& is required
     * ml::Application app(videoMode, "My App",
     *                      ml::WindowManager::getWindow());
     * @endcode
     *
     * @note The window is heap-allocated via @c new and the pointer is stored
     *       in a local static. Do not call @c delete on the returned reference.
     *
     * @return Reference to the single shared @c sf::RenderWindow.
     */
    inline sf::RenderWindow& getWindow()
    {
        static sf::RenderWindow* window = new sf::RenderWindow;
        return *window;
    }

} // namespace ml::WindowManager

#endif // WINDOWMANAGER_H
