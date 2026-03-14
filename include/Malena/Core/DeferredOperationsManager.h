//
// Created by Dave Smith on 12/16/25.
//

//
// DeferredOperationsManager.h
//

#ifndef MALENA_DEFERREDOPERATIONSMANAGER_H
#define MALENA_DEFERREDOPERATIONSMANAGER_H

#include <functional>
#include <vector>

namespace ml {

/**
 * CRTP Base class for managers that need to defer operations during iteration/processing
 * Each derived class gets its own static state (busyDepth, pendingOperations)
 *
 * Usage:
 *   class MyManager : public DeferredOperationsManager<MyManager> {
 *       void myMethod() {
 *           beginBusy();
 *           // ... do work ...
 *           endBusy();  // Auto-processes pending
 *       }
 *   };
 */
template<typename Derived>
class DeferredOperationsManager {
protected:
    inline static int busyDepth = 0;
    inline static std::vector<std::function<void()>> pendingOperations;

    /**
     * Mark that we're busy (iterating/firing/processing)
     * Nested calls are supported (depth counter)
     */
    static void beginBusy();

    /**
     * Mark that we're done, process pending if depth reaches 0
     */
    static void endBusy();

    /**
     * Defer an operation if busy, execute immediately if safe
     */
    static void deferOrExecute(std::function<void()> operation);

public:
    /**
     * Check if currently busy (useful for external checks)
     */
    static bool isBusy();

    /**
     * Force process all pending operations
     * Usually called automatically by endBusy(), but can be called manually
     */
    static void processPending();

    /**
     * Clear all pending operations without executing them
     * Use when shutting down or resetting
     */
    static void clearPending();
};

} // namespace ml

#include "../../../src/ECS/Core/DeferredOperationsManager.tpp"
#endif // MALENA_DEFERREDOPERATIONSMANAGER_H

