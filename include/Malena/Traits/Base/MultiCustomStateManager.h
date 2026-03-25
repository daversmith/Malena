// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MULTICUSTOMSTATEMANAGER_H
#define MULTICUSTOMSTATEMANAGER_H

#include <type_traits>
#include <tuple>
#include <functional>
#include <Malena/Utilities/TypeExtraction.h>

namespace ml
{
    // =========================================================================
    // SingleStateManager
    // =========================================================================

    /**
     * @brief State machine for a single enum type.
     *
     * Stores one current state value of type @c StateEnum and fires optional
     * enter/exit callbacks on every transition. One @c SingleStateManager base
     * is inherited per distinct state enum.
     *
     * @note This class is an implementation detail of @c MultiCustomStateManager.
     *       User code accesses states through the component's unified API.
     *
     * @tparam StateEnum An @c enum @c class type representing the possible states.
     *
     * @see MultiCustomStateManager, GatherStates
     */
    template<typename StateEnum>
    class SingleStateManager
    {
        StateEnum _currentState{};
        std::function<void(StateEnum)> _onEnter;
        std::function<void(StateEnum)> _onExit;

    public:
        /**
         * @brief Transition to a new state.
         *
         * If an exit callback is registered, it is called with the current
         * state before the transition. If an enter callback is registered,
         * it is called with the new state after the transition.
         *
         * @param newState The state to transition into.
         */
        void setState(StateEnum newState)
        {
            if (_onExit)  _onExit(_currentState);
            _currentState = newState;
            if (_onEnter) _onEnter(_currentState);
        }

        /**
         * @brief Return the current state value.
         * @return The active state.
         */
        StateEnum getState() const { return _currentState; }

        /**
         * @brief Return @c true if the current state equals @p state.
         * @param state State value to compare against.
         * @return @c true if currently in @p state.
         */
        bool isState(StateEnum state) const { return _currentState == state; }

        /**
         * @brief Register a callback invoked when any state is entered.
         *
         * The callback receives the newly entered state as its argument.
         * Only one enter callback is active at a time; calling this again
         * replaces the previous one.
         *
         * @param cb Callback invoked with the new state after each transition.
         */
        void onStateEnter(std::function<void(StateEnum)> cb) { _onEnter = std::move(cb); }

        /**
         * @brief Register a callback invoked when any state is exited.
         *
         * The callback receives the state being left as its argument.
         * Only one exit callback is active at a time; calling this again
         * replaces the previous one.
         *
         * @param cb Callback invoked with the old state before each transition.
         */
        void onStateExit(std::function<void(StateEnum)> cb)  { _onExit  = std::move(cb); }
    };

    // =========================================================================
    // MultiCustomStateManager
    // =========================================================================

    /**
     * @brief Aggregates state machines for multiple enum types into one class.
     *
     * @c MultiCustomStateManager<StateEnums...> inherits one
     * @c SingleStateManager per enum in the pack and lifts all their methods
     * into a single overload set. This allows a component to call a single
     * @c setState, @c getState, or @c isState overloaded on enum type, without
     * casting to a specific base.
     *
     * It is instantiated by @c GatherStates, which collects all @c State enums
     * from a component's manifest and from every trait mixed in via
     * @c ComponentWith.
     *
     * ### Example (automatic — no user action required)
     * @code
     * struct MyManifest { enum class State { Normal, Selected }; };
     * // Draggable also has: enum class State { FREE, LOCK_X, LOCK_Y };
     *
     * class MyWidget : public ml::ComponentWith<MyManifest, Draggable> {};
     *
     * MyWidget w;
     * w.setState(MyManifest::State::Selected);
     * w.setState(ml::Draggable::State::LOCK_X);
     *
     * w.onStateEnter([](MyManifest::State s) { /* react to MyWidget state changes *\/ });
     * @endcode
     *
     * @tparam StateEnums Zero or more @c enum @c class types, one per state machine.
     *                    The empty specialization is used when no custom states
     *                    are declared.
     *
     * @see SingleStateManager, GatherStates, ComponentCore
     */
    template<typename... StateEnums>
    /**
     * @brief MultiCustomStateManager.
     * @ingroup TraitsBase
     */
    class MultiCustomStateManager : public SingleStateManager<StateEnums>...
    {
    public:
        using SingleStateManager<StateEnums>::setState...;
        using SingleStateManager<StateEnums>::getState...;
        using SingleStateManager<StateEnums>::isState...;
        using SingleStateManager<StateEnums>::onStateEnter...;
        using SingleStateManager<StateEnums>::onStateExit...;
    };

    /**
     * @brief No-op specialization used when no custom states are declared.
     *
     * Adds no storage. The variadic @c ... absorb accidental calls so that
     * @c using declarations in @c ComponentCore still compile.
     */
    template<>
    class MultiCustomStateManager<>
    {
    public:
        void setState(...)     {}
        void onStateEnter(...) {}
        void onStateExit(...)  {}
    };

    // =========================================================================
    // State extraction and gather helpers
    // =========================================================================

    /// @cond INTERNAL

    /**
     * @brief Extracts the @c State enum from a manifest struct.
     *
     * Yields @c void if the manifest has no @c State member type.
     */
    template<typename M, typename = void>
    struct extract_ManifestState { using type = void; };

    template<typename M>
    struct extract_ManifestState<M, std::void_t<typename M::State>>
    { using type = typename M::State; };

    // ── Void filtering (mirrors the pattern in MultiCustomFlaggable) ──────────

    template<typename T, typename Acc>
    struct AppendStateIfNotVoid { using type = Acc; };

    template<typename T, typename... Acc>
    struct AppendStateIfNotVoid<T, std::tuple<Acc...>>
    {
        using type = std::conditional_t<
            std::is_void_v<T>,
            std::tuple<Acc...>,
            std::tuple<Acc..., T>
        >;
    };

    template<typename... Ts>
    struct FilterVoidStates;

    template<>
    struct FilterVoidStates<> { using type = std::tuple<>; };

    template<typename T, typename... Rest>
    struct FilterVoidStates<T, Rest...>
    {
        using type = typename AppendStateIfNotVoid<
            T,
            typename FilterVoidStates<Rest...>::type
        >::type;
    };

    template<typename Tuple>
    struct TupleToMultiStateManager;

    template<typename... StateEnums>
    struct TupleToMultiStateManager<std::tuple<StateEnums...>>
    { using type = MultiCustomStateManager<StateEnums...>; };

    /// @endcond

    // =========================================================================
    // GatherStates
    // =========================================================================

    /**
     * @brief Collects all @c State enums from a component manifest and its traits.
     *
     * @c GatherStates<ComponentManifest, Traits...>::type resolves to a
     * @c MultiCustomStateManager instantiation whose template parameters are
     * exactly the non-void @c State enums found in:
     * - @c ComponentManifest::State (if it exists)
     * - @c TraitManifest::State for each @c Trait that has a @c manifest_type
     *
     * @c ComponentCore inherits this type to get a single, unified state
     * store for the entire component hierarchy. @c void entries are filtered
     * out automatically.
     *
     * @tparam ComponentManifest The component's own manifest, or @c void.
     * @tparam Traits            The extra traits mixed into the component.
     *
     * @see MultiCustomStateManager, ComponentCore, GatherFlags
     */
    template<typename ComponentManifest, typename... Traits>
    struct GatherStates
    {
        using AllStatesTuple = typename FilterVoidStates<
            typename extract_ManifestState<ComponentManifest>::type,
            typename extract_ManifestState<
                typename extract_TraitManifest<Traits>::type
            >::type...
        >::type;

        using type = typename TupleToMultiStateManager<AllStatesTuple>::type;
    };

} // namespace ml

#endif // MULTICUSTOMSTATEMANAGER_H
