//
// Created by Dave R. Smith on 3/4/25.
//

#ifndef FLAGGABLE_H
#define FLAGGABLE_H

#pragma once

#include <map>
#include <iostream>
#include <Malena/Utilities/EnumClassHash.h>
#include "Malena/Utilities/Flag.h"

namespace ml
{
    /**
     * @brief Enum-keyed boolean flag store.
     *
     * @c FlagManager<StateEnum> maintains a @c std::map from @c StateEnum
     * values to @c bool and exposes the standard five-method flag API.
     * It is the backing store for the @c Flaggable trait (@c ml::Flag)
     * and is used directly by @c CustomFlaggable for manifest-declared flags.
     *
     * All flags default to @c false on first access.
     *
     * ### Typical use via @c Flaggable (system flags)
     * @code
     * // System flags are available on every ml::Core object
     * myComponent.setFlag(ml::Flag::DRAGGABLE);
     * myComponent.checkFlag(ml::Flag::HOVERED);
     * myComponent.toggleFlag(ml::Flag::FOCUSED);
     * @endcode
     *
     * ### Typical use via @c ComponentWith (custom flags)
     * @code
     * struct MyManifest {
     *     enum class Flag { Selected, Disabled };
     * };
     * class MyWidget : public ml::ComponentWith<MyManifest> {};
     *
     * MyWidget w;
     * w.enableFlag(MyManifest::Flag::Selected);
     * if (w.checkFlag(MyManifest::Flag::Selected)) { ... }
     * @endcode
     *
     * @tparam StateEnum An @c enum @c class type used as flag keys.
     *                   Typically @c ml::Flag for system flags, or a manifest
     *                   @c Flag enum for custom flags.
     *
     * @see Flaggable, CustomFlaggable, MultiCustomFlaggable, ml::Flag
     */
    template<typename StateEnum>
    /**
     * @brief FlagManager.
     * @ingroup Resources
     */
    class FlagManager
    {
    public:
        /// The enum type this manager stores flags for.
        using State = StateEnum;

    private:
        std::map<State, bool> _states;

    public:
        FlagManager();
        FlagManager(const FlagManager& stateful);

        /**
         * @brief Return the current value of a flag.
         *
         * Returns @c false for any flag that has never been explicitly set.
         *
         * @param state The flag to query.
         * @return @c true if the flag is enabled; @c false otherwise.
         */
        bool checkFlag(State state) const;

        /**
         * @brief Set a flag to @c true.
         *
         * @param state The flag to enable.
         */
        void enableFlag(State state);

        /**
         * @brief Set a flag to @c false.
         *
         * @param state The flag to disable.
         */
        void disableFlag(State state);

        /**
         * @brief Flip a flag between @c true and @c false.
         *
         * @param state The flag to toggle.
         */
        void toggleFlag(State state);

        /**
         * @brief Set a flag to an explicit value.
         *
         * @param state  The flag to modify.
         * @param status @c true to enable, @c false to disable.
         */
        void setFlag(State state, bool status);
    };

} // namespace ml

#include "../../../src/Resources/FlagManager.tpp"
#endif // FLAGGABLE_H
