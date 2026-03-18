//
// Created by Dave Smith on 10/5/25.
//

#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <type_traits>
#include <functional>

namespace ml
{
    /// @cond INTERNAL

    /**
     * @brief Selects the callback signature based on whether @c T is @c void.
     *
     * When @c T is a real enum, callbacks receive the enum value.
     * When @c T is @c void (the no-op specialization), callbacks take no argument.
     */
    template<typename T>
    struct StateCallback
    {
        using type = std::function<void(T)>;
    };

    template<>
    struct StateCallback<void>
    {
        using type = std::function<void()>;
    };

    /// @endcond

    /**
     * @brief Single-enum state machine with enter/exit callbacks.
     *
     * @c StateManager<StateEnum> stores one active state value and fires
     * optional callbacks on every transition. It is used in two places:
     *
     * - @c Customizable<Manifest> inherits @c StateManager<Manifest::State>
     *   to give traits their own state machine.
     * - @c MultiCustomStateManager uses @c SingleStateManager (a similar type)
     *   to aggregate multiple state machines for components.
     *
     * All methods are guarded by @c std::enable_if so they exist only when
     * @c StateEnum is not @c void. The @c void specialization at the bottom
     * of this file is a no-op that compiles cleanly when the manifest declares
     * no @c State enum.
     *
     * ### Usage
     * @code
     * struct MyManifest {
     *     enum class State { Idle, Running, Paused };
     * };
     *
     * class MyComponent : public ml::ComponentWith<MyManifest> {};
     *
     * MyComponent c;
     *
     * c.onStateEnter([](MyManifest::State s){
     *     if (s == MyManifest::State::Running) startAnimation();
     * });
     *
     * c.onStateExit([](MyManifest::State s){
     *     if (s == MyManifest::State::Running) stopAnimation();
     * });
     *
     * c.setState(MyManifest::State::Running); // fires exit(Idle), then enter(Running)
     * c.isState(MyManifest::State::Running);  // true
     * c.getState();                           // MyManifest::State::Running
     * @endcode
     *
     * @tparam StateEnum An @c enum @c class type representing the possible states.
     *                   Defaults to @c void (selects the no-op specialization).
     *
     * @see Customizable, MultiCustomStateManager, GatherStates
     */
    template<typename StateEnum = void>
    /**
     * @brief StateManager.
     * @ingroup Resources
     */
    class StateManager
    {
    private:
        std::conditional_t<
            !std::is_void_v<StateEnum>,
            StateEnum,
            int
        > _currentState{};

        typename StateCallback<StateEnum>::type _onEnterCallback;
        typename StateCallback<StateEnum>::type _onExitCallback;

    public:
        /**
         * @brief Construct with an explicit initial state.
         *
         * @param initialState The state to start in. The enter callback is
         *                     @b not fired during construction.
         */
        template<typename T = StateEnum>
        explicit StateManager(std::enable_if_t<!std::is_void_v<T>, T> initialState);

        /// Default constructor — initial state is the zero-value of @c StateEnum.
        StateManager() = default;

        /**
         * @brief Register a callback invoked after each state transition.
         *
         * The callback receives the newly entered state. Only one enter
         * callback is active at a time; calling this again replaces it.
         *
         * @param callback Function called with the new state after each transition.
         */
        template<typename T = StateEnum>
        std::enable_if_t<!std::is_void_v<T>>
        onStateEnter(std::function<void(T)> callback);

        /**
         * @brief Register a callback invoked before each state transition.
         *
         * The callback receives the state being left. Only one exit callback
         * is active at a time; calling this again replaces it.
         *
         * @param callback Function called with the old state before each transition.
         */
        template<typename T = StateEnum>
        std::enable_if_t<!std::is_void_v<T>>
        onStateExit(std::function<void(T)> callback);

        /**
         * @brief Transition to a new state.
         *
         * Fires the exit callback with the current state, updates the stored
         * state to @p newState, then fires the enter callback with the new state.
         * Either callback may be unset (nullptr), in which case it is skipped.
         *
         * @param newState The state to transition into.
         */
        template<typename T = StateEnum>
        std::enable_if_t<!std::is_void_v<T>>
        setState(T newState);

        /**
         * @brief Return the current state value.
         *
         * @return The active @c StateEnum value.
         */
        template<typename T = StateEnum>
        std::enable_if_t<!std::is_void_v<T>, T>
        getState() const;

        /**
         * @brief Return @c true if currently in @p state.
         *
         * @param state The state value to compare against.
         * @return @c true if the current state equals @p state.
         */
        template<typename T = StateEnum>
        std::enable_if_t<!std::is_void_v<T>, bool>
        isState(T state) const;
    };

    /**
     * @brief No-op specialization used when @c Manifest has no @c State enum.
     *
     * Declares no members so there is no ambiguity when this base is combined
     * with other @c StateManager specializations in a multiple-inheritance chain.
     */
    template<>
    class StateManager<void> {};

} // namespace ml

#include "../../../src/Resources/StateManager.tpp"
#endif // STATEMANAGER_H
