// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef APPMANAGER_H
#define APPMANAGER_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/CoreManager.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

#include <Malena/Engine/App/Lifecycle.h>
namespace ml
{
    /**
     * @brief Drives the application main loop and coordinates all framework managers.
     * @ingroup EngineApp
     *
     * @c AppManager owns the SFML render window and runs the core application
     * lifecycle:
     *
     * 1. Calls @c Lifecycle::onInit() then @c Lifecycle::onReady() before
     *    entering the loop.
     * 2. Each frame, polls SFML events and distributes them via @c fireInputEvents().
     * 3. Fires the per-frame update event via @c fireUpdateEvents().
     * 4. Clears the window, calls @c draw(), and presents the frame.
     *
     * @c AppManager implements the @c Manager interface, so it participates in
     * the same event-distribution protocol as other framework managers.
     *
     * ### Architecture modes
     * The @c Architecture enum lets the framework know which structural style
     * the application uses.
     *
     * | Value | Meaning |
     * |-------|---------|
     * | @c MVC | Model-View-Controller — default for most UI applications |
     * | @c EDA | Event-Driven Architecture — pure message/event flow |
     * | @c ECS | Entity-Component-System — activates the ECS subsystem |
     *
     * ### Typical use
     * Most applications do not instantiate @c AppManager directly. Instead,
     * inherit from @c ml::Application, which wraps @c AppManager into a
     * single convenient base class.
     *
     * For the common case, subclass @c ml::Application and override
     * @c onInit() and @c onReady() directly:
     * @code
     * class MyApp : public ml::Application
     * {
     * public:
     *     MyApp() : ml::Application(1280, 720, 32, "My App") {}
     *
     *     void onInit()  override { _box.setSize({200.f, 100.f}); addComponent(_box); }
     *     void onReady() override { _box.onClick([]{ std::cout << "clicked!\n"; }); }
     * private:
     *     ml::Rectangle _box;
     * };
     * @endcode
     *
     * @see Application, Manager, WindowManager
     */
    class MALENA_API AppManager : public Lifecycle, public CoreManager<Core>
    {
    private:
        sf::RenderWindow* window = nullptr;

        /// @cond INTERNAL
        inline static bool        _isDrawing = false;
        inline static AppManager* _instance  = nullptr;
        inline static std::vector<std::function<void()>> _deferredUnloads;
        /// @endcond

    public:
        /**
         * @brief Construct an @c AppManager and create the SFML window.
         *
         * @c onInit() and @c onReady() are called by @c run() before the main
         * loop begins, not in the constructor. The @p window parameter defaults
         * to @c WindowManager::getWindow() so the framework's centralized window
         * is used unless an explicit one is provided.
         *
         * @param videoMode  SFML video mode (resolution + bit depth).
         * @param title      Window title string.
         * @param window     Render window to use. Defaults to the framework window.
         */
    	AppManager(const sf::VideoMode& videoMode,
				   const std::string& title,
				   sf::RenderWindow& window = WindowManager::getWindow());

        /**
         * @brief Enter the main loop and run until the window is closed.
         *
         * Each iteration of the loop:
         * -# Polls all pending SFML events and passes each to @c fireInputEvents().
         * -# Calls @c fireUpdateEvents() for the per-frame update tick.
         * -# Clears the window, calls @c draw() on all registered components,
         *    and displays the frame.
         *
         * Returns when the SFML window is closed or @c window.close() is called.
         */
        void run();

        virtual ~AppManager() = default;

        /// @cond INTERNAL
        static bool isDrawing() { return _isDrawing; }
        static void deferUnload(std::function<void()> op)
        {
            _deferredUnloads.push_back(std::move(op));
        }

        /**
         * @brief Return the single running @c AppManager instance.
         *
         * Set on construction. Follows the same pattern as
         * @c WindowManager::getWindow() — allows framework subsystems
         * (e.g. @c PluginManager) to remove components from the application's
         * @c CoreManager without requiring a direct reference.
         */
        static AppManager& get() { return *_instance; }
        /// @endcond

    private:
        void fireInputEvents(const std::optional<sf::Event>& event);
        void fireUpdateEvents();
        void draw();
    };

} // namespace ml

#endif // UIMANAGER_H
