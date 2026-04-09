// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 10/3/25.
//

#ifndef ENUMCLASSHASH_H
#define ENUMCLASSHASH_H
/**
 * @file EnumClassHash.h
 * @ingroup Utilities
 */

#include <Malena/Core/malena_export.h>

namespace ml{
    struct EnumClassHash {
        template <typename T>
        std::size_t operator()(T t) const {
            return static_cast<std::size_t>(t);
        }
    };}
#endif //ENUMCLASSHASH_H
