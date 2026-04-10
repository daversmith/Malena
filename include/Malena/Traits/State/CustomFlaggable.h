// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 10/4/25.
//

#ifndef MALENA_CUSTOMFLAGGABLE_H
#define MALENA_CUSTOMFLAGGABLE_H

#include <Malena/Core/malena_export.h>
#include <unordered_map>
#include <iostream>
#include <Malena/Utilities/EnumClassHash.h>

namespace ml
{
    /**
     * @brief Provides enum-keyed boolean flag storage for a single flag set.
     * @ingroup Traits
     *
     * @c CustomFlaggale<CustomFlagEnum> maintains an @c unordered_map from
     * enum values to @c bool, exposing a consistent API for querying and
     * mutating those flags. It is the low-level storage layer used by
     * @c MultiCustomFlaggable to hold each individual flag set gathered from
     * a component's manifest and traits.
     *
     * All methods are guarded by @c std::enable_if so they exist in the
     * overload set only when @c CustomFlagEnum is not @c void. This allows
     * @c CustomFlaggale to be safely used as a base class even when the
     * manifest declares no flags — see the @c void specialization below.
     *
     * ### Typical use (via @c ComponentWith)
     * You will rarely instantiate @c CustomFlaggale directly. Instead, declare
     * a @c Flags enum in your manifest and call the flag methods on your
     * component:
     * @code
     * struct MyManifest {
     *     enum class Flags { Selected, Hovered, Disabled };
     * };
     *
     * class MyWidget : public ml::ComponentWith<MyManifest> {};
     *
     * MyWidget w;
     * w.enableFlag(MyManifest::Flags::Selected);
     * if (w.checkFlag(MyManifest::Flags::Selected)) { ... }
     * w.disableFlag(MyManifest::Flags::Selected);
     * @endcode
     *
     * @tparam CustomFlagEnum An @c enum @c class type used as flag keys.
     *                        Use @c void when no custom flags are needed
     *                        (selects the no-op specialization).
     *
     * @see MultiCustomFlaggable, Flaggable, ComponentCore
     */
    template<typename CustomFlagEnum>
    class CustomFlaggable
    {
        std::conditional_t<
            !std::is_void_v<CustomFlagEnum>,
            std::unordered_map<CustomFlagEnum, bool, EnumClassHash>,
            std::unordered_map<int, bool>
        > _custom_states;

    public:
        /**
         * @brief Return the current value of a flag.
         *
         * Returns @c false for any flag that has never been explicitly set.
         *
         * @param state The flag to query.
         * @return @c true if the flag is set; @c false otherwise.
         */
        template<typename T = CustomFlagEnum>
        std::enable_if_t<!std::is_void_v<T>, bool>
        checkFlag(T state) const;

        /**
         * @brief Set a flag to an explicit value.
         *
         * @param state  The flag to modify.
         * @param status @c true to enable the flag, @c false to disable it.
         */
        template<typename T = CustomFlagEnum>
        std::enable_if_t<!std::is_void_v<T>>
        setFlag(T state, bool status);

        /**
         * @brief Set a flag to @c true.
         *
         * @param state The flag to enable.
         */
        template<typename T = CustomFlagEnum>
        std::enable_if_t<!std::is_void_v<T>>
        enableFlag(T state);

        /**
         * @brief Set a flag to @c false.
         *
         * @param state The flag to disable.
         */
        template<typename T = CustomFlagEnum>
        std::enable_if_t<!std::is_void_v<T>>
        disableFlag(T state);

        /**
         * @brief Flip a flag between @c true and @c false.
         *
         * @param state The flag to toggle.
         */
        template<typename T = CustomFlagEnum>
        std::enable_if_t<!std::is_void_v<T>>
        toggleFlag(T state);
    };

    /**
     * @brief No-op specialization used when no custom flags are needed.
     *
     * Instantiated automatically when a manifest omits the @c Flags enum.
     * Produces no storage overhead and no methods, allowing @c GatherFlags
     * to safely include it in the inheritance chain without changing the
     * public API.
     */
    template<>
    class CustomFlaggable<void> {};

} // namespace ml

#include "../../../../src/Traits/State/CustomFlaggable.tpp"
#endif // CUSTOMSTATEFUL_H
