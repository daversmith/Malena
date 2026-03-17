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
     * @brief Primary entry point for Malena applications.
     *
     * @c Application combines @c AppManager (the main loop runner) and
     * @c UIController (the initialization and event-registration contract)
     * into a single class. For most programs, this is the only framework
     * base class you need to inherit from.
     *
     * ### Simple subclassing pattern
     * Override @c initialization() to create and register components, then
     * override @c registerEvents() to attach callbacks. @c run() starts the
     * main loop.
     *
     * @code
     * class MyApp : public ml::Application
     * {
     * public:
     *     MyApp() : ml::Application(1280, 720, 32, "My App") {}
     *
     *     void initialization() override
     *     {
     *         _box.setSize({200.f, 100.f});
     *         _box.setPosition({100.f, 100.f});
     *         addComponent(_box);
     *     }
     *
     *     void registerEvents() override
     *     {
     *         _box.onClick([]{ std::cout << "clicked!\n"; });
     *     }
     *
     * private:
     *     ml::Rectangle _box;
     * };
     *
     * int main()
     * {
     *     MyApp app;
     *     app.run();
     * }
     * @endcode
     *
     * ### Separate controller pattern
     * For larger projects where you want to keep the runner and the UI logic
     * in separate classes, pass a @c UIController reference to the constructor
     * that accepts one. @c Application then delegates @c initialization() and
     * @c registerEvents() to that controller.
     *
     * @code
     * MyController controller;
     * ml::Application app(sf::VideoMode({1280, 720}), "My App", controller);
     * app.run();
     * @endcode
     *
     * @see AppManager, UIController, Controller
     */
    class Application : public AppManager, public UIController
    {
    public:
        /**
         * @brief Construct with a separate @c UIController and an explicit video mode.
         *
         * Use this constructor when the initialization and event-registration
         * logic live in a dedicated @c UIController subclass rather than in
         * @c Application itself.
         *
         * @param videoMode  SFML video mode (resolution + bit depth).
         * @param title      Window title string.
         * @param appLogic   Controller that provides the @c initialization() and
         *                   @c registerEvents() implementations.
         * @param window     Render window to use. Defaults to the framework
         *                   window managed by @c WindowManager.
         */
        Application(const sf::VideoMode& videoMode,
                    const std::string& title,
                    UIController& appLogic,
                    sf::RenderWindow& window = WindowManager::getWindow());

        /**
         * @brief Construct from pixel dimensions and bit depth.
         *
         * The most common constructor for subclasses that implement
         * @c initialization() and @c registerEvents() directly.
         *
         * @param screenWidth  Window width in pixels.
         * @param screenHeight Window height in pixels.
         * @param bitDepth     Color depth (typically 32).
         * @param title        Window title string.
         */
        Application(unsigned int screenWidth,
                    unsigned int screenHeight,
                    unsigned int bitDepth,
                    const std::string& title);

        /**
         * @brief Construct from an SFML video mode, using @c *this as the controller.
         *
         * Suitable for @c Application subclasses that implement their own
         * @c initialization() and @c registerEvents(). The current object is
         * passed as the @c UIController to @c AppManager.
         *
         * @param videoMode  SFML video mode (resolution + bit depth).
         * @param title      Window title string.
         */
        Application(const sf::VideoMode& videoMode,
                    const std::string& title);

        /**
         * @brief Register a @c Core object with the application's component manager.
         *
         * After registration the component is included in the framework's
         * update and draw loop. Call this from @c initialization().
         *
         * @note Do not copy or move a component after it has been registered —
         *       the manager stores a pointer to the original object.
         *
         * @param component The component to register.
         */
        void addComponent(Core& component);
    };

} // namespace ml

#endif // APPLICATION_H
