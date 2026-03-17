#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#pragma once

#include <Malena/Core/CoreAdapter.h>
#include <Malena/Engine/App/Controller.h>

namespace ml
{
    /**
     * @brief Concrete @c Controller base with a per-frame update hook.
     *
     * @c UIController extends @c Controller with two additions:
     *
     * - A @c CoreAdapter proxy that gives this controller a lightweight
     *   framework identity (position, flags, event subscriptions) without
     *   being a full drawable component. This proxy is what @c AppManager
     *   uses to fire the @c "update" event at the controller level.
     * - @c onUpdate() — a convenience method for registering a per-frame
     *   callback directly on the controller, useful for logic that doesn't
     *   belong to any single component.
     *
     * ### Typical subclassing
     * @code
     * class MyController : public ml::UIController
     * {
     * public:
     *     void initialization() override
     *     {
     *         _button.setSize({120.f, 40.f});
     *         _button.setPosition({100.f, 100.f});
     *         addComponent(_button);
     *     }
     *
     *     void registerEvents() override
     *     {
     *         _button.onClick([]{ std::cout << "clicked\n"; });
     *
     *         onUpdate([this]{
     *             // per-frame controller logic
     *         });
     *     }
     *
     * private:
     *     ml::Rectangle _button;
     * };
     * @endcode
     *
     * ### Standalone use (separate controller + runner)
     * @c UIController can be passed to the @c Application constructor that
     * accepts a controller reference, keeping the application runner and
     * the UI logic in separate objects:
     * @code
     * MyController controller;
     * ml::Application app(sf::VideoMode({1280, 720}), "My App", controller);
     * app.run();
     * @endcode
     *
     * For simpler programs, inherit from @c ml::Application directly —
     * it already inherits @c UIController and @c AppManager together.
     *
     * @see Controller, Application, AppManager, CoreAdapter
     */
    class UIController : public Controller
    {
        CoreAdapter proxy;

    public:
        using Controller::Controller;

        void initialization() override = 0;

        void registerEvents() override = 0;

        /**
         * @brief Register a callback invoked every frame during the update tick.
         *
         * Equivalent to calling @c onUpdate on a component, but scoped to the
         * controller itself. Use this for frame-level logic that is not tied
         * to a specific component — for example, polling game state, advancing
         * a timer, or orchestrating scene transitions.
         *
         * @param f Callback with no arguments, invoked once per frame.
         */
        void onUpdate(std::function<void()> f);

        ~UIController() override = default;
    };

} // namespace ml

#endif // GAMELOGIC_H
