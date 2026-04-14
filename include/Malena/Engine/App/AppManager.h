// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef MALENA_APPMANAGER_H
#define MALENA_APPMANAGER_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/CoreManager.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <SFML/Graphics.hpp>
#include <functional>
#include <optional>
#include <string>
#include <cstdint>

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
     * 2. Each frame, measures delta time, polls SFML events and distributes them
     *    via @c fireInputEvents().
     * 3. Fires the per-frame update event via @c fireUpdateEvents() (skipped when paused).
     * 4. Clears the window with the configured background colour, calls the pre-render
     *    hook, draws all visible components, calls the post-render hook, and presents
     *    the frame.
     * 5. At the end of each iteration, flushes any pending deferred-unload operations.
     *
     * ### Window configuration
     * Most window properties can be set before @c run() or at any time during the
     * loop via dedicated setters:
     *
     * | Setter | Effect |
     * |--------|--------|
     * | @c setBackgroundColor() | Colour passed to @c window.clear() each frame |
     * | @c setFramerateLimit()  | Cap the frame rate (0 = unlimited) |
     * | @c setVSync()           | Toggle vertical synchronisation |
     * | @c setTitle()           | Change the window title at runtime |
     * | @c setIcon()            | Set the window icon from an @c sf::Image |
     * | @c setWindowPosition()  | Move the window on the desktop |
     * | @c setWindowStyle()     | Recreate the window with new decoration flags |
     *
     * ### Lifecycle hooks
     * | Setter | Called |
     * |--------|--------|
     * | @c onPreRender()  | After @c clear(), before drawing components |
     * | @c onPostRender() | After drawing all components, before @c display() |
     * | @c onClose()      | When the OS close button is pressed; return @c false to cancel |
     * | @c onResize()     | When the window is resized |
     *
     * ### Pause / resume
     * @c pause() suspends @c fireUpdateEvents() while leaving the render loop running,
     * so the window stays responsive and the last rendered frame remains visible.
     * @c resume() restores normal operation.
     *
     * ### Delta time
     * @c AppManager::getDeltaTime() returns the elapsed time (in seconds) of the
     * previous frame, updated at the start of each iteration. Components can call
     * this from any @c onUpdate callback.
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
     * @see Application, Lifecycle, WindowManager
     */
    class MALENA_API AppManager : public Lifecycle, public CoreManager<Core>
    {
    public:
        /**
         * @brief Architectural style hint passed at construction.
         *
         * Informs the framework which structural pattern the application
         * follows. Defaults to @c MVC.
         */
        enum Architecture
        {
            MVC, ///< Model-View-Controller
            EDA, ///< Event-Driven Architecture
            ECS  ///< Entity-Component-System
        };

    private:
        sf::RenderWindow* window = nullptr;

        Architecture      _architecture;
        sf::Color         _clearColor   { sf::Color::Black };
        sf::Clock         _clock;
        bool              _paused       { false };

        // Stored for window recreation (setWindowStyle)
        sf::VideoMode     _videoMode;
        std::string       _title;
        std::uint32_t     _windowStyle;
        unsigned int      _framerateLimit { 60 };

        std::function<void()>                           _preRenderHook;
        std::function<void()>                           _postRenderHook;
        std::function<bool()>                           _closeHandler;
        std::function<void(unsigned int, unsigned int)> _resizeHandler;

        /// @cond INTERNAL
        inline static bool        _isDrawing = false;
        inline static float       _deltaTime = 0.f;
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
         * @param videoMode    SFML video mode (resolution + bit depth).
         * @param title        Window title string.
         * @param window       Render window to use. Defaults to the framework window.
         * @param architecture Structural pattern hint. Defaults to @c MVC.
         * @param windowStyle  SFML window style flags (e.g. @c sf::Style::Default,
         *                     @c sf::Style::None, @c sf::Style::Fullscreen).
         *                     Defaults to @c sf::Style::Default.
         */
        AppManager(const sf::VideoMode& videoMode,
                   const std::string& title,
                   sf::RenderWindow& window = WindowManager::getWindow(),
                   Architecture architecture = MVC,
                   std::uint32_t windowStyle = sf::Style::Default);

        /**
         * @brief Enter the main loop and run until the window is closed.
         *
         * Each iteration of the loop:
         * -# Measures delta time for the previous frame.
         * -# Polls all pending SFML events (resize and close are handled internally).
         * -# Calls @c fireUpdateEvents() for the per-frame update tick (skipped when paused).
         * -# Clears the window, invokes @c onPreRender, draws all visible components,
         *    invokes @c onPostRender, and presents the frame.
         * -# Flushes any pending deferred-unload operations.
         *
         * Returns when the SFML window is closed or @c window.close() is called.
         */
        void run();

        virtual ~AppManager() = default;

        // ── Window appearance ─────────────────────────────────────────────────

        /**
         * @brief Set the colour used to clear the window at the start of each frame.
         *
         * @param color Any @c sf::Color value. Defaults to @c sf::Color::Black.
         */
        void setBackgroundColor(sf::Color color);

        /**
         * @brief Change the window title at runtime.
         *
         * @param title New UTF-8 window title string.
         */
        void setTitle(const std::string& title);

        /**
         * @brief Set the window icon from an @c sf::Image.
         *
         * @param icon An @c sf::Image loaded with your desired icon pixels.
         */
        void setIcon(const sf::Image& icon);

        /**
         * @brief Move the window to a position on the desktop.
         *
         * @param x Horizontal position in desktop pixels.
         * @param y Vertical position in desktop pixels.
         */
        void setWindowPosition(int x, int y);

        /**
         * @brief Recreate the window with new decoration-style flags.
         *
         * Equivalent to calling @c sf::Window::create again with the stored
         * video-mode and title but a new style. Any previously configured
         * framerate limit is re-applied automatically.
         *
         * @param style Combination of @c sf::Style flags, e.g.
         *              @c sf::Style::Titlebar | @c sf::Style::Close.
         */
        void setWindowStyle(std::uint32_t style);

        // ── Timing ────────────────────────────────────────────────────────────

        /**
         * @brief Cap the frame rate.
         *
         * Calls @c sf::Window::setFramerateLimit. Pass @c 0 to remove the cap.
         *
         * @param limit Maximum frames per second.
         */
        void setFramerateLimit(unsigned int limit);

        /**
         * @brief Enable or disable vertical synchronisation.
         *
         * @param enabled @c true to enable VSync, @c false to disable.
         */
        void setVSync(bool enabled);

        /**
         * @brief Return the elapsed time of the previous frame in seconds.
         *
         * Updated at the start of every main-loop iteration.
         * Safe to call from any @c onUpdate callback.
         */
        static float getDeltaTime() { return _deltaTime; }

        // ── Pause / resume ────────────────────────────────────────────────────

        /**
         * @brief Suspend per-frame update events.
         *
         * After @c pause(), @c fireUpdateEvents() is skipped each iteration.
         * The render loop continues running so the window stays responsive
         * and the last rendered frame remains visible.
         */
        void pause();

        /** @brief Resume update events after a @c pause(). */
        void resume();

        /** @brief Return @c true if the application is currently paused. */
        bool isPaused() const { return _paused; }

        // ── Lifecycle hooks ───────────────────────────────────────────────────

        /**
         * @brief Register a callback invoked after @c clear() and before drawing components.
         *
         * Use this to draw background layers, debug overlays, or ImGui frames
         * that should appear beneath the component layer.
         *
         * @param hook Callable with signature @c void().
         */
        void onPreRender(std::function<void()> hook);

        /**
         * @brief Register a callback invoked after all components are drawn and before @c display().
         *
         * Use this for post-processing effects, HUD overlays, or any content
         * that must appear on top of all components.
         *
         * @param hook Callable with signature @c void().
         */
        void onPostRender(std::function<void()> hook);

        /**
         * @brief Register a callback invoked when the OS close button is pressed.
         *
         * @param handler Callable with signature @c bool(). Return @c true to
         *                allow the window to close (default behaviour), or
         *                @c false to cancel the close (e.g. show a "save changes?" dialog).
         */
        void onClose(std::function<bool()> handler);

        /**
         * @brief Register a callback invoked whenever the window is resized.
         *
         * @param handler Callable with signature @c void(unsigned int width, unsigned int height).
         */
        void onResize(std::function<void(unsigned int, unsigned int)> handler);

        // ── Architecture ──────────────────────────────────────────────────────

        /** @brief Return the architectural mode set at construction. */
        Architecture getArchitecture() const { return _architecture; }

        // ── Internal ──────────────────────────────────────────────────────────

        /// @cond INTERNAL
        static bool isDrawing() { return _isDrawing; }

        static void deferUnload(std::function<void()> op)
        {
            _deferredUnloads.push_back(std::move(op));
        }

        /**
         * @brief Return the single running @c AppManager instance.
         *
         * Set on construction. Allows framework subsystems (e.g. @c PluginManager)
         * to remove components from the application's @c CoreManager without
         * requiring a direct reference.
         */
        static AppManager& get() { return *_instance; }
        /// @endcond

    private:
        void fireInputEvents(const std::optional<sf::Event>& event);
        void fireUpdateEvents();
        void draw();
        void flushDeferredUnloads();
    };

} // namespace ml

#endif // MALENA_APPMANAGER_H
