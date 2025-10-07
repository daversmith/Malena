//
// Created by Dave Smith on 10/5/25.
//

#ifndef STATEMANAGER_CPP
#define STATEMANAGER_CPP
#include <Malena/Managers/StateManager.h>
namespace ml {

    template<typename StateEnum>
    template<typename T>
    StateManager<StateEnum>::StateManager(std::enable_if_t<!std::is_void_v<T>, T> initialState)
        : _currentState(initialState) {}

    template<typename StateEnum>
    StateManager<StateEnum>::StateManager()
        : _currentState(0) {}

    template<typename StateEnum>
    template<typename T>
    std::enable_if_t<!std::is_void_v<T>>
    StateManager<StateEnum>::onStateEnter(std::function<void(T)> callback) {
        _onEnterCallback = callback;
    }

    template<typename StateEnum>
    template<typename T>
    std::enable_if_t<!std::is_void_v<T>>
    StateManager<StateEnum>::onStateExit(std::function<void(T)> callback) {
        _onExitCallback = callback;
    }

    template<typename StateEnum>
    template<typename T>
    std::enable_if_t<!std::is_void_v<T>>
    StateManager<StateEnum>::setState(T newState) {
        if (_currentState != newState) {
            if (_onExitCallback) {
                _onExitCallback(_currentState);
            }
            _currentState = newState;
            if (_onEnterCallback) {
                _onEnterCallback(newState);
            }
        }
    }

    template<typename StateEnum>
    template<typename T>
    std::enable_if_t<!std::is_void_v<T>, T>
    StateManager<StateEnum>::getState() const {
        return _currentState;
    }

    template<typename StateEnum>
    template<typename T>
    std::enable_if_t<!std::is_void_v<T>, bool>
    StateManager<StateEnum>::isState(T state) const {
        return _currentState == state;
    }

} // namespace ml

#endif // STATEMANAGER_CPP