// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_HAS_SETSIZE_H
#define MALENA_HAS_SETSIZE_H

#pragma once

#include <SFML/System/Vector2.hpp>
#include <type_traits>

namespace ml::detail
{
    /// @cond INTERNAL
    template<typename T, typename = void>
    struct has_setSize : std::false_type {};

    template<typename T>
    struct has_setSize<T, std::void_t<
        decltype(std::declval<T&>().setSize(std::declval<sf::Vector2f>()))
    >> : std::true_type {};
    /// @endcond

} // namespace ml::detail

#endif // MALENA_HAS_SETSIZE_H
