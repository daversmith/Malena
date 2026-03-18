//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef UIMANAGER_H
#define UIMANAGER_H

#pragma once

#include <SFML/Graphics.hpp>
#include <Malena/Engine/App/UIController.h>
#include <Malena/Core/Manager.h>
#include <Malena/Core/CoreManager.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <string>

namespace ml
{
    /**
     * @brief Drives the application main loop and coordinates all framework managers.
      * @ingroup EngineApp
     *
     * @c AppManager owns the SFML render window and runs the core application
     * lifecycle:
     *
     * 1. Calls @c UIController::initialization() and @c UIController::registerEvents()
     *    before entering the loop.
     * 2. Each frame, polls SFML events and distributes them via @c fireInputEvents().
     * 3. Fires the per-frame update event via @c fireUpdateEvents().
     * 4. Clears the window, calls @c draw(), and presents the frame.
     *
     * @c AppManager implements the @c Manager interface, so it participates in
     * the same event-distribution protocol as other framework managers.
     *
     * ### Architecture modes
     * The @c Architecture enum lets the framework know which structural style
     * the application uses. This may influence how managers initialize or
     * which subsystems are activated.
     *
     * | Value | Meaning |
     * |-------|---------|
     * | @c MVC | Model-View-Controller — default for most UI applications |
     * | @c EDA | Event-Driven Architecture — pure message/event flow |
     * | @c ECS | Entity-Component-System — activates the ECS subsystem |
     *
     * ### Typical use
     * Most applications do not instantiate @c AppManager directly. Instead,
     * inherit from @c ml::Application, which combines @c AppManager and
     * @c UIController into a single convenient base class.
     *
     * For projects that separate the runner from the UI logic:
     * @code
     * MyController controller;
     * ml::AppManager runner(sf::VideoMode({1280, 720}), "My App", controller);
     * runner.run();
     * @endcode
     *
     * @see Application, UIController, Manager, WindowManager
     */
    class AppManager : public Manager
    {
    private:
        UIController*    uiController;
        sf::RenderWindow* window = nullptr;

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

        /**
         * @brief Construct an @c AppManager with an explicit controller and window.
         *
         * Calls @c controller.initialization() then @c controller.registerEvents()
         * before returning. The @p window parameter defaults to
         * @c WindowManager::getWindow() so that the framework's centralized
         * window is used unless you explicitly provide your own.
         *
         * @param videoMode     SFML video mode (resolution + bit depth).
         * @param title         Window title string.
         * @param appLogic      Controller that provides @c initialization() and
         *                      @c registerEvents() implementations.
         * @param window        Render window to use. Defaults to the framework window.
         * @param architecture  Structural pattern hint. Defaults to @c MVC.
         */
        AppManager(const sf::VideoMode& videoMode,
                   const std::string& title,
                   UIController& appLogic,
                   sf::RenderWindow& window = WindowManager::getWindow(),
                   Architecture architecture = MVC);

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
        void run() override;

        virtual ~AppManager() = default;

    private:
        /**
         * @brief Distribute one SFML event to all registered managers and components.
         *
         * Called once per polled event. Forwards the event through
         * @c EventsManager so that subscribed components can react to
         * input, window resize, focus changes, etc.
         *
         * @param event The current SFML event.
         */
        void fireInputEvents(const std::optional<sf::Event>& event) override;

        /**
         * @brief Fire the per-frame @c "update" event to all subscribers.
         *
         * Called once per frame after all input events have been processed.
         */
        void fireUpdateEvents() override;

        /**
         * @brief Draw all registered components to the render window.
         *
         * Iterates @c CoreManager and calls @c draw() on each registered
         * component in registration order.
         */
        void draw() override;
    };

} // namespace ml

#endif // UIMANAGER_H
