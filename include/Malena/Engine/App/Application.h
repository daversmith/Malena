#ifndef APPLICATION_H
#define APPLICATION_H
#pragma once

#include <Malena/Engine/App/UIController.h>
#include <Malena/Engine/App/AppManager.h>
#include <Malena/Core/CoreManager.h>
#include <string>

namespace ml
{
    /**
     * @class Application
     * @brief Combines application logic and the engine loop into a single class.
     *
     * `Application` serves as the main entry point for simple Malena programs.
     * It inherits from both `AppManager` and `UIController`, allowing a subclass
     * to define application behavior directly by overriding `initialization()`
     * and `registerEvents()`.
     *
     * For projects that prefer a cleaner separation between the application runner
     * and the UI logic, use the constructor that accepts a separate `UIController`.
     *
     * @code
     * class MyApp : public ml::Application
     * {
     * public:
     *     MyApp() : ml::Application(1280, 720, 32, "My App") {}
     *
     *     void initialization() override
     *     {
     *         addComponent(mySprite);
     *     }
     *
     *     void registerEvents() override
     *     {
     *         mySprite.onClick([]{ std::cout << "clicked!\n"; });
     *     }
     *
     * private:
     *     ml::Sprite mySprite;
     * };
     *
     * int main()
     * {
     *     MyApp app;
     *     app.run();
     * }
     * @endcode
     */
    class Application : public AppManager, public UIController
    {
    public:
        /**
         * @brief Creates an application that uses a separate UI controller.
         *
         * Use this constructor when the application's event registration and
         * initialization logic are implemented in a dedicated `UIController`
         * object rather than directly in a subclass of `Application`.
         *
         * @param videoMode The video mode used to create the render window.
         * @param title The title displayed in the application window.
         * @param appLogic The controller responsible for initialization and event registration.
         * @param window The render window used by the application.
         */
        Application(const sf::VideoMode &videoMode, const std::string &title, UIController &appLogic,
                    sf::RenderWindow &window = WindowManager::getWindow());

        /**
         * @brief Creates an application from width, height, and bit depth.
         *
         * This constructor is intended for subclasses of `Application` that
         * implement `initialization()` and `registerEvents()` directly.
         *
         * @param screenWidth The width of the application window in pixels.
         * @param screenHeight The height of the application window in pixels.
         * @param bitDepth The color depth of the window.
         * @param title The title displayed in the application window.
         */
        Application(unsigned int screenWidth, unsigned int screenHeight, unsigned int bitDepth,
                    const std::string &title);

        /**
         * @brief Creates an application from an SFML video mode.
         *
         * This constructor uses the current `Application` object as the UI
         * controller, making it suitable for subclasses that define their
         * own initialization and event registration behavior.
         *
         * @param videoMode The video mode used to create the render window.
         * @param title The title displayed in the application window.
         */
        Application(const sf::VideoMode &videoMode, const std::string &title);

        /**
         * @brief Registers a core component with the application.
         *
         * Registered components are managed by `CoreManager` and become part of
         * the application's update and render flow.
         *
         * @param component The component to register.
         */
        void addComponent(Core &component);
    };

} // namespace ml

#endif // APPLICATION_H