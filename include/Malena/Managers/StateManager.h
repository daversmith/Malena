//
// Created by Dave Smith on 10/5/25.
//

#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <type_traits>
#include <functional>

namespace ml {

    template<typename T>
    struct StateCallback {
        using type = std::function<void(T)>;
    };

    template<>
    struct StateCallback<void> {
        using type = std::function<void()>;
    };

    // In StateManager
    template<typename StateEnum = void>
    class StateManager {
    private:
        std::conditional_t<
            !std::is_void_v<StateEnum>,
            StateEnum,
            int
        > _currentState;

        typename StateCallback<StateEnum>::type _onEnterCallback;
        typename StateCallback<StateEnum>::type _onExitCallback;

    public:
        // Constructor with initial state
        template<typename T = StateEnum>
        explicit StateManager(std::enable_if_t<!std::is_void_v<T>, T> initialState);

        // Default constructor
        StateManager();

        // Register onStateEnter callback
        template<typename T = StateEnum>
        std::enable_if_t<!std::is_void_v<T>>
        onStateEnter(std::function<void(T)> callback);

        // Register onStateExit callback
        template<typename T = StateEnum>
        std::enable_if_t<!std::is_void_v<T>>
        onStateExit(std::function<void(T)> callback);

        // Set state with transition callbacks
        template<typename T = StateEnum>
        std::enable_if_t<!std::is_void_v<T>>
        setState(T newState);

        // Get current state
        template<typename T = StateEnum>
        std::enable_if_t<!std::is_void_v<T>, T>
        getState() const;

        // Check if in specific state
        template<typename T = StateEnum>
        std::enable_if_t<!std::is_void_v<T>, bool>
        isState(T state) const;
    };

} // namespace ml

#include "../../../src/Managers/StateManager.cpp"

#endif // STATEMANAGER_H