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
    static void beginBusy() {
        ++busyDepth;
    }

    /**
     * Mark that we're done, process pending if depth reaches 0
     */
    static void endBusy() {
        --busyDepth;
        if (busyDepth == 0) {
            processPending();
        }
    }

    /**
     * Defer an operation if busy, execute immediately if safe
     */
    static void deferOrExecute(std::function<void()> operation) {
        if (busyDepth > 0) {
            pendingOperations.push_back(std::move(operation));
        } else {
            operation();
        }
    }

public:
    /**
     * Check if currently busy (useful for external checks)
     */
    static bool isBusy() {
        return busyDepth > 0;
    }

    /**
     * Force process all pending operations
     * Usually called automatically by endBusy(), but can be called manually
     */
    static void processPending() {
        if (pendingOperations.empty()) return;

        auto ops = std::move(pendingOperations);
        pendingOperations.clear();

        for (auto& op : ops) {
            op();
        }
    }

    /**
     * Clear all pending operations without executing them
     * Use when shutting down or resetting
     */
    static void clearPending() {
        pendingOperations.clear();
    }
};

} // namespace ml

#endif // MALENA_DEFERREDOPERATIONSMANAGER_H

