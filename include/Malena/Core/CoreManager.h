//
// CoreManager.h  (previously ComponentsManager.h)
//

#ifndef COMPONENTSMANAGER_H
#define COMPONENTSMANAGER_H

#pragma once

#include <vector>
#include <Malena/Core/Core.h>
#include <Malena/Core/DeferredOperationsManager.h>

namespace ml
{
    /**
     * @brief Static, type-safe collection manager for @c Core-derived objects.
     *
     * @c CoreManager<T> owns a static list of @c T* pointers and provides
     * add, remove, and query operations. It inherits
     * @c DeferredOperationsManager so that removals requested while the
     * collection is being iterated are queued and applied safely once
     * iteration completes — preventing iterator invalidation crashes.
     *
     * Because the underlying storage is @c inline @c static, all instances
     * of @c CoreManager<T> share a single list per type @c T. This means
     * the manager behaves as a singleton per type without requiring an
     * explicit instance to be passed around.
     *
     * ### Typical use
     * @code
     * // Add a component to the managed collection
     * CoreManager<ml::UIComponent>::addComponent(myRect);
     *
     * // Remove safely from anywhere, including inside an event callback
     * CoreManager<ml::UIComponent>::removeComponent(myRect);
     * @endcode
     *
     * ### Safety contract
     * - @c addComponent and @c removeComponent are safe to call at any time,
     *   including from within an event or update callback that is currently
     *   iterating the collection.
     * - Do @b not copy or move a @c T object after it has been registered.
     *   The collection stores raw pointers; moving the object invalidates them.
     *
     * @tparam T A @c Core-derived type whose instances will be managed.
     *
     * @see DeferredOperationsManager, Core, Application
     */
    template<typename T>
    /**
     * @brief CoreManager.
     * @ingroup Core
     */
    class CoreManager : public DeferredOperationsManager<CoreManager<T>>
    {
        inline static std::vector<T*> _components;

    public:
        /**
         * @brief Return a read-only view of all currently registered objects.
         *
         * The returned reference is valid until the next structural modification
         * (add or remove). Do not store the reference across frames.
         *
         * @return Const reference to the internal component pointer vector.
         */
        [[nodiscard]] static const std::vector<T*>& getComponents();

        /**
         * @brief Register a @c T object with this manager.
         *
         * After registration, the object will receive @c update() and @c draw()
         * calls each frame (for managers that drive those methods).
         *
         * @param component The object to register. Must outlive its registration.
         */
        static void addComponent(T& component);

        /**
         * @brief Unregister a @c T object by reference.
         *
         * Safe to call from inside an event or update callback. If the
         * collection is currently being iterated, the removal is deferred
         * until iteration completes.
         *
         * @param component The object to remove.
         * @return @c true if the object was found and removed (or queued
         *         for removal); @c false if it was not registered.
         */
        static bool removeComponent(T& component);

        /**
         * @brief Unregister a @c T object by pointer.
         *
         * Equivalent to the reference overload. Passing @c nullptr is safe
         * and returns @c false.
         *
         * @param component Pointer to the object to remove.
         * @return @c true if found and removed (or queued); @c false otherwise.
         */
        static bool removeComponent(T* component);

        /**
         * @brief Remove all registered objects.
         *
         * If the collection is currently being iterated, the clear is deferred.
         * After this call (or after the deferred clear executes), @c getComponents()
         * returns an empty vector.
         */
        static void clear();

        virtual ~CoreManager();

    private:
        /**
         * @brief Execute the actual pointer removal from the collection.
         *
         * Called directly when the collection is idle, or scheduled as a
         * pending operation by @c DeferredOperationsManager when it is busy.
         *
         * @param component Pointer to the object to erase.
         */
        static void doRemoveComponent(T* component);
    };

} // namespace ml

#include "../../../src/Core/CoreManager.cpp"
#endif // COMPONENTSMANAGER_H
