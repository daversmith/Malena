//
// Lifecycle.h
//

#ifndef LIFECYCLE_H
#define LIFECYCLE_H

#pragma once

namespace ml
{
    /**
     * @brief Trait that adds one-time initialization lifecycle hooks to any class.
     * @ingroup Traits
     *
     * @c Lifecycle provides two virtual hooks that bracket the startup phase of
     * an object. Both have empty default implementations — override only what
     * you need.
     *
     * | Hook        | When it fires                                          |
     * |-------------|--------------------------------------------------------|
     * | @c onInit() | Once, immediately after construction, before the first frame |
     * | @c onReady()| Once, after @c onInit(), once all components are registered |
     *
     * The distinction between the two hooks mirrors the pattern established by
     * frameworks such as Angular and iOS UIKit:
     * - @c onInit() is the place to perform setup that does not depend on other
     *   components being fully registered (load resources, configure properties).
     * - @c onReady() is the place to wire logic that requires all sibling objects
     *   to already exist (cross-component references, initial state calculations).
     *
     * ### Usage
     * @code
     * // ml::Application already inherits Lifecycle — just override the hooks
     * class MyApp : public ml::Application
     * {
     * public:
     *     MyApp() : ml::Application(1280, 720, 32, "My App") {}
     *
     * protected:
     *     void onInit() override
     *     {
     *         _button.setSize({120.f, 40.f});
     *         _button.setPosition({100.f, 100.f});
     *         addComponent(_button);
     *     }
     *
     *     void onReady() override
     *     {
     *         // _button is fully registered — safe to reference it here
     *         _button.onClick([]{ std::cout << "clicked\n"; });
     *     }
     *
     * private:
     *     ml::Rectangle _button;
     * };
     * @endcode
     *
     * @c Lifecycle is independent of @c SceneLifecycle. Opt into both if your
     * class is a scene that also needs one-time initialization hooks:
     * @code
     * class MyScene : public ml::Scene,
     *                 public ml::Lifecycle,
     *                 public ml::SceneLifecycle
     * { ... };
     * @endcode
     *
     * @see SceneLifecycle, Updatable, Application
     */
    class Lifecycle
    {
    public:
        /**
         * @brief Called once after construction, before the first frame.
         *
         * Override to perform initial setup — configure component properties,
         * load resources, and register objects with the framework. At this point
         * no other sibling components are guaranteed to be fully registered.
         */
        virtual void onInit() {}

        /**
         * @brief Called once after @c onInit(), when all components are registered.
         *
         * Override to wire logic that depends on other components already existing —
         * event callbacks, cross-component bindings, or derived initial state.
         */
        virtual void onReady() {}

        virtual ~Lifecycle() = default;
    };

} // namespace ml

#endif // LIFECYCLE_H
