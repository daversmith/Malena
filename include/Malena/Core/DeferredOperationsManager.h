//
// Created by Dave Smith on 12/16/25.
//

#ifndef MALENA_DEFERREDOPERATIONSMANAGER_H
#define MALENA_DEFERREDOPERATIONSMANAGER_H

#include <functional>
#include <vector>

namespace ml
{
    /**
     * @brief CRTP base that gives a manager safe deferred-operation support.
     *
     * Several Malena managers own collections that are iterated during event
     * firing, update ticks, or draw calls. If user code (e.g., a click
     * callback) tries to remove an element from the collection while it is
     * being iterated, the result is undefined behavior — typically a crash.
     *
     * @c DeferredOperationsManager solves this with a simple pattern:
     *
     * - Wrap any iteration with @c beginBusy() / @c endBusy().
     * - Wrap any destructive operation (removal, unsubscribe, unload) with
     *   @c deferOrExecute().
     * - When @c endBusy() decrements the depth counter to zero, all queued
     *   operations are flushed automatically.
     *
     * Because the pattern uses CRTP, each derived manager gets its own
     * independent static @c busyDepth counter and @c pendingOperations queue.
     * @c EventsManager, @c MessageManager, @c CoreManager, and @c PluginManager
     * all inherit from this class for exactly that reason.
     *
     * Nested calls are fully supported: @c beginBusy() increments the depth
     * and @c endBusy() only flushes when the depth returns to zero.
     *
     * ### Implementing a new deferred manager
     * @code
     * class MyManager : public DeferredOperationsManager<MyManager>
     * {
     *     static void fireAll()
     *     {
     *         beginBusy();
     *         for (auto* item : _items)
     *             item->notify();
     *         endBusy(); // flushes any remove() calls made during notify()
     *     }
     *
     *     static void remove(Item* item)
     *     {
     *         deferOrExecute([item]{ doRemove(item); });
     *     }
     * };
     * @endcode
     *
     * @tparam Derived The concrete manager class (CRTP parameter).
     *
     * @see CoreManager, EventsManager, MessageManager, PluginManager
     */
    template<typename Derived>
    class DeferredOperationsManager
    {
    protected:
        /// Iteration nesting depth. Operations are deferred while this is > 0.
        inline static int busyDepth = 0;

        /// Queue of operations pending until the current iteration completes.
        inline static std::vector<std::function<void()>> pendingOperations;

        /**
         * @brief Signal that iteration has begun.
         *
         * Increments the busy-depth counter. Multiple nested calls are
         * safe — operations remain deferred until the outermost @c endBusy()
         * is reached.
         */
        static void beginBusy();

        /**
         * @brief Signal that iteration has ended; flush pending operations.
         *
         * Decrements the busy-depth counter. When the counter reaches zero,
         * all operations in @c pendingOperations are executed in FIFO order
         * and the queue is cleared.
         */
        static void endBusy();

        /**
         * @brief Execute @p operation now if safe, otherwise queue it.
         *
         * If @c busyDepth > 0 the operation is appended to
         * @c pendingOperations and will run when the current iteration
         * completes. If the manager is idle the operation executes
         * immediately.
         *
         * @param operation The callable to execute or defer.
         */
        static void deferOrExecute(std::function<void()> operation);

    public:
        /**
         * @brief Return @c true if the manager is currently iterating.
         *
         * Can be used by external code that needs to know whether a
         * destructive operation will be deferred.
         *
         * @return @c true when @c busyDepth > 0.
         */
        static bool isBusy();

        /**
         * @brief Flush all pending operations immediately.
         *
         * Normally called automatically by @c endBusy(). Provided as a
         * public escape hatch for unusual teardown sequences.
         *
         * @warning Calling this while @c isBusy() is @c true can cause
         *          iterator invalidation in the currently running loop.
         *          Only call manually when you are certain it is safe.
         */
        static void processPending();

        /**
         * @brief Discard all pending operations without executing them.
         *
         * Use during application shutdown or full reset when executing
         * the queued operations would be unsafe (e.g., the objects they
         * reference have already been destroyed).
         */
        static void clearPending();
    };

} // namespace ml

#include "../../../src/ECS/Core/DeferredOperationsManager.tpp"
#endif // MALENA_DEFERREDOPERATIONSMANAGER_H
