// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 3/11/25.
//

#ifndef MANAGER_H
#define MANAGER_H

#pragma once
#include <optional>
namespace ml
{
    /**
     * @brief Abstract base class for all Malena managers.
      * @ingroup Core
     *
     * A @c Manager is responsible for driving one subsystem through the
     * application main loop. Every concrete manager must implement the four
     * lifecycle steps that @c AppManager calls each frame:
     *
     * 1. @c fireInputEvents — distribute the current SFML event to the subsystem
     * 2. @c fireUpdateEvents — perform per-frame logic updates
     * 3. @c run — execute any subsystem-specific per-frame work
     * 4. @c draw — render the subsystem's output to the window
     *
     * Managers are typically statically allocated singletons (e.g.,
     * @c EventsManager, @c ComponentsManager) rather than instances passed
     * around by pointer, but the interface is expressed as a virtual class
     * so that @c AppManager can hold a heterogeneous list of registered managers.
     *
     * @see AppManager, EventsManager, CoreManager
     */
    class Manager
    {
    public:
        virtual ~Manager() = default;

        /**
         * @brief Distribute an SFML input event to this subsystem.
         *
         * Called once per frame for each event returned by
         * @c sf::Window::pollEvent. Managers that respond to keyboard,
         * mouse, or window events process them here.
         *
         * @param event The SFML event, wrapped in @c std::optional.
         *              An empty optional indicates the end of the event queue
         *              (i.e., the update-only tick).
         */
        virtual void fireInputEvents(const std::optional<sf::Event>& event) = 0;

        /**
         * @brief Fire the per-frame update event across all subscribers.
         *
         * Called once per frame after all input events have been processed.
         * Use this to drive animation, polling, or any time-based logic.
         */
        virtual void fireUpdateEvents() = 0;

        /**
         * @brief Execute subsystem-specific per-frame work.
         *
         * Provides an additional hook beyond @c fireUpdateEvents for
         * managers that need to perform internal bookkeeping separate
         * from event distribution.
         */
        virtual void run() = 0;

        /**
         * @brief Render the subsystem's visual output to the window.
         *
         * Called once per frame after @c run(). Implementations should
         * call @c window.draw() on any objects this manager is responsible
         * for drawing.
         */
        virtual void draw() = 0;
    };

} // namespace ml

#endif // MANAGER_H
