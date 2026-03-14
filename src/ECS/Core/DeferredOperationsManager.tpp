//
// Created by Dave Smith on 12/16/25.
//

#include <Malena/Core/DeferredOperationsManager.h>

namespace ml {
    template<typename Derived>
    void DeferredOperationsManager<Derived>::beginBusy() {
        ++busyDepth;
    }

    template<typename Derived>
    void DeferredOperationsManager<Derived>::endBusy() {
        --busyDepth;
        if (busyDepth == 0) {
            processPending();
        }
    }

    template<typename Derived>
    void DeferredOperationsManager<Derived>::deferOrExecute(std::function<void()> operation) {
        if (busyDepth > 0) {
            pendingOperations.push_back(std::move(operation));
        } else {
            operation();
        }
    }

    template<typename Derived>
    bool DeferredOperationsManager<Derived>::isBusy() {
        return busyDepth > 0;
    }

    template<typename Derived>
    void DeferredOperationsManager<Derived>::processPending() {
        if (pendingOperations.empty()) return;

        auto ops = std::move(pendingOperations);
        pendingOperations.clear();

        for (auto& op : ops) {
            op();
        }
    }

    template<typename Derived>
    void DeferredOperationsManager<Derived>::clearPending() {
        pendingOperations.clear();
    }
} // ml