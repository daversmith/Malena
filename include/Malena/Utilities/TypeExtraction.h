//
// Created by Dave Smith on 3/10/26.
//

#ifndef TYPEEXTRACTION_H
#define TYPEEXTRACTION_H

namespace ml
{
    template<typename T, typename = void>
    struct extract_Flags { using type = void; };

    template<typename T>
    struct extract_Flags<T, std::void_t<typename T::Flags>> {
        using type = typename T::Flags;
    };

    template<typename T, typename = void>
    struct extract_State { using type = void; };

    template<typename T>
    struct extract_State<T, std::void_t<typename T::State>> {
        using type = typename T::State;
    };
}

#endif //TYPEEXTRACTION_H
