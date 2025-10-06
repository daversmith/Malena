//
// Created by Dave Smith on 10/4/25.
//

#ifndef CUSTOMSTATEFUL_H
#define CUSTOMSTATEFUL_H
#include <map>
#include <iostream>
#include <Malena/Utilities/EnumClassHash.h>

namespace ml {
    template<typename CustomStateEnum>
    class CustomStateful {
        std::conditional_t<  // C++14, alias for std::conditional
                !std::is_void_v<CustomStateEnum>,  // C++17, alias for std::is_void<T>::value
                std::map<CustomStateEnum, bool, EnumClassHash>,
                std::map<int, bool>
            > _custom_states;

    public:
        template<typename T = CustomStateEnum>
                std::enable_if_t<!std::is_void_v<T>, bool>
                checkState(T state) const;

        template<typename T = CustomStateEnum>
        std::enable_if_t<!std::is_void_v<T>>
        setState(T state, bool status);

        template<typename T = CustomStateEnum>
        std::enable_if_t<!std::is_void_v<T>>
        enableState(T state);

        template<typename T = CustomStateEnum>
        std::enable_if_t<!std::is_void_v<T>>
        disableState(T state);

        template<typename T = CustomStateEnum>
        std::enable_if_t<!std::is_void_v<T>>
        toggleState(T state);
    };

} // ml

#endif //CUSTOMSTATEFUL_H
