//
// Created by Dave Smith on 10/4/25.
//

#include "../../include/Malena/Traits/CustomStateful.h"

namespace ml {
    template<typename CustomStateEnum>
    template<typename T>
    std::enable_if_t<!std::is_void_v<T>, bool>
    CustomStateful<CustomStateEnum>::checkState(T state) const
    {
        auto it = _custom_states.find(state);
        return it != _custom_states.end() && it->second;
    }
    template<typename CustomStateEnum>
    template<typename T>
    std::enable_if_t<!std::is_void_v<T>>
    CustomStateful<CustomStateEnum>::enableState(T state)
    {
        _custom_states.at(state)= true;
    }
    template<typename CustomStateEnum>
    template<typename T>
    std::enable_if_t<!std::is_void_v<T>>
    CustomStateful<CustomStateEnum>::disableState(T state)
    {
        _custom_states.at(state) = false;
    }
    template<typename CustomStateEnum>
    template<typename T>
    std::enable_if_t<!std::is_void_v<T>>
    CustomStateful<CustomStateEnum>::toggleState(T state)
    {
        _custom_states.at(state) = !_custom_states.at(state);
    }
    template<typename CustomStateEnum>
    template<typename T>
    std::enable_if_t<!std::is_void_v<T>>
    CustomStateful<CustomStateEnum>::setState(T state, bool status)
    {
        _custom_states.at(state) = status;
    }
} // ml