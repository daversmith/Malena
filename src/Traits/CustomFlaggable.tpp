//
// Created by Dave Smith on 10/4/25.
//

#ifndef CUSTOMFLAGGABLE_CPP
#define CUSTOMFLAGGABLE_CPP
#include "../../include/Malena/Traits/CustomFlaggable.h"

namespace ml {
    template<typename CustomStateEnum>
    template<typename T>
    std::enable_if_t<!std::is_void_v<T>, bool>
    CustomFlaggable<CustomStateEnum>::checkFlag(T state) const
    {
        auto it = _custom_states.find(state);
        return it != _custom_states.end() && it->second;
    }
    template<typename CustomStateEnum>
    template<typename T>
    std::enable_if_t<!std::is_void_v<T>>
    CustomFlaggable<CustomStateEnum>::enableFlag(T state)
    {
        _custom_states.at(state)= true;
    }
    template<typename CustomStateEnum>
    template<typename T>
    std::enable_if_t<!std::is_void_v<T>>
    CustomFlaggable<CustomStateEnum>::disableFlag(T state)
    {
        _custom_states.at(state) = false;
    }
    template<typename CustomStateEnum>
    template<typename T>
    std::enable_if_t<!std::is_void_v<T>>
    CustomFlaggable<CustomStateEnum>::toggleFlag(T state)
    {
        _custom_states.at(state) = !_custom_states.at(state);
    }
    template<typename CustomStateEnum>
    template<typename T>
    std::enable_if_t<!std::is_void_v<T>>
    CustomFlaggable<CustomStateEnum>::setFlag(T state, bool status)
    {
        _custom_states.at(state) = status;
    }
} // ml
#endif