//
// Created by Dave Smith on 10/3/25.
//

#ifndef ENUMCLASSHASH_H
#define ENUMCLASSHASH_H

namespace ml{
    struct EnumClassHash {
        template <typename T>
        std::size_t operator()(T t) const {
            return static_cast<std::size_t>(t);
        }
    };}
#endif //ENUMCLASSHASH_H
