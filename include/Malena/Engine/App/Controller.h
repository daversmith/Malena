//
// Created by Dave Smith on 3/8/25.
//

#ifndef CONTROLLER_H
#define CONTROLLER_H

#pragma once

namespace ml
{
    /**
     * @brief Abstract interface for application initialization and event registration.
     *
     * @c Controller defines the two-phase lifecycle contract that every Malena
     * application entry point must fulfill:
     *
     * 1. @c initialization() — called once at startup to create and register
     *    components, load resources, and set up initial state.
     * 2. @c registerEvents() — called immediately after @c initialization() to
     *    attach event callbacks (@c onClick, @c onHover, @c onMessage, etc.)
     *    to the components created in the first phase.
     *
     * Separating these two phases ensures that all objects exist before any
     * callback tries to reference them, avoiding order-of-initialization issues.
     *
     * @c UIController extends @c Controller with a @c CoreAdapter proxy and
     * an @c onUpdate convenience method. @c Application inherits from both
     * @c AppManager and @c UIController, collapsing the full lifecycle into a
     * single subclass.
     *
     * @see UIController, Application
     */
    class Controller
    {
    public:
        /**
         * @brief Create and register all components and resources.
         *
         * Implement this method to construct the objects your application needs
         * and register them with the appropriate managers (e.g., via
         * @c addComponent). This method runs before @c registerEvents(), so
         * every object you plan to subscribe to will already exist by the time
         * callbacks are attached.
         */
        virtual void initialization() = 0;

        /**
         * @brief Attach event callbacks to components and framework objects.
         *
         * Implement this method to wire up @c onClick, @c onHover, @c onUpdate,
         * @c onMessage, and any other subscriptions. Called immediately after
         * @c initialization() by @c AppManager before the main loop begins.
         */
        virtual void registerEvents() = 0;

        virtual ~Controller() = default;
    };

} // namespace ml

#endif // CONTROLLER_H
