// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// EnumKey.h
//

#ifndef MALENA_ENUMKEY_H
#define MALENA_ENUMKEY_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <string>
#include <typeinfo>
#include <type_traits>

namespace ml
{
    /**
     * @brief Utility for generating unique, stable string keys from enum values.
     * @ingroup Utilities
     *
     * @c EnumKey converts any scoped or unscoped enum value into a
     * @c std::string key by combining the enum type's mangled name with
     * the enum's integer value. The resulting key is:
     *
     * - **Guaranteed unique** — different enum types produce different
     *   @c typeid().name() strings; different values produce different
     *   integers. No collision is possible.
     * - **General purpose** — works for any enum, not just event enums.
     * - **Human-readable** — keys like @c "N2ml5EventE::0" are debuggable.
     *
     * ### Performance note
     * Keys are @c std::string — map lookups are slightly slower than
     * @c size_t but collision-free and correct at any scale. If Malena
     * grows to a scale where string lookup cost matters, switch to a
     * hash-combine approach (e.g. @c boost::hash_combine) and benchmark.
     *
     * ### Usage
     * @code
     * std::string key = ml::EnumKey::get(ml::Event::CLICK);
     * std::string key = ml::EnumKey::get(MyManifest::Event::SCORE_CHANGED);
     * @endcode
     *
     * @note Keys are stable within a single program run but are NOT
     *       guaranteed stable across runs, platforms, or compiler versions
     *       because @c typeid().name() is implementation-defined.
     *       Do not persist or serialize EnumKey values.
     *
     * @see EventManager, Fireable, Unsubscribable
     */
    struct EnumKey
    {
        /**
         * @brief Generate a unique string key for an enum value.
         *
         * Combines @c typeid(EnumType).name() with the integer value of
         * @p value via @c "::". Guaranteed unique per (type, value) pair.
         *
         * @tparam EnumType Any enum type (scoped or unscoped). Enforced
         *                  at compile time via @c std::is_enum_v.
         * @param  value    The enum value to convert to a key.
         * @return A unique @c std::string key for this (type, value) pair.
         */
        template<typename EnumType>
        static std::string get(EnumType value)
        {
            static_assert(std::is_enum_v<EnumType>,
                "[Malena] EnumKey::get — template parameter must be an enum type.");

            return std::string(typeid(EnumType).name())
                 + "::"
                 + std::to_string(static_cast<int>(value));
        }

        /**
         * @brief Generate a unique string key for an enum type alone.
         *
         * Returns a key representing the enum type without a specific value.
         * Useful when grouping all events of a given enum type.
         *
         * @tparam EnumType Any enum type.
         * @return The @c typeid name for @c EnumType.
         */
        template<typename EnumType>
        static std::string typeKey()
        {
            static_assert(std::is_enum_v<EnumType>,
                "[Malena] EnumKey::typeKey — template parameter must be an enum type.");

            return std::string(typeid(EnumType).name());
        }
    };

} // namespace ml

#endif // MALENA_ENUMKEY_H
