//
// Created by Dave Smith on 10/4/25.
//

#ifndef CUSTOMFLAGGABLE_H
#define CUSTOMFLAGGABLE_H
#include <unordered_map>
#include <iostream>
#include <Malena/Utilities/EnumClassHash.h>

namespace ml {

    template<typename CustomFlagEnum>
    class CustomFlaggable {
        std::conditional_t<  // C++14, alias for std::conditional
                !std::is_void_v<CustomFlagEnum>,  // C++17, alias for std::is_void<T>::value
                std::unordered_map<CustomFlagEnum, bool, EnumClassHash>,
                std::unordered_map<int, bool>
            > _custom_states;

    public:
        template<typename T = CustomFlagEnum>
                std::enable_if_t<!std::is_void_v<T>, bool>
                checkFlag(T state) const;

        template<typename T = CustomFlagEnum>
        std::enable_if_t<!std::is_void_v<T>>
        setFlag(T state, bool status);

        template<typename T = CustomFlagEnum>
        std::enable_if_t<!std::is_void_v<T>>
        enableFlag(T state);

        template<typename T = CustomFlagEnum>
        std::enable_if_t<!std::is_void_v<T>>
        disableFlag(T state);

        template<typename T = CustomFlagEnum>
        std::enable_if_t<!std::is_void_v<T>>
        toggleFlag(T state);
    };


} // ml
#include "../../../src/Traits/CustomFlaggable.cpp"
#endif //CUSTOMSTATEFUL_H
