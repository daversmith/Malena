#ifndef MULTICUSTOMSTATEMANAGER_H
#define MULTICUSTOMSTATEMANAGER_H

#include <type_traits>
#include <tuple>
#include <functional>
#include <Malena/Utilities/TypeExtraction.h>

namespace ml
{
    // ── SingleStateManager — one state store per enum type ────────────────────
    template<typename StateEnum>
    class SingleStateManager
    {
        StateEnum _currentState{};
        std::function<void(StateEnum)> _onEnter;
        std::function<void(StateEnum)> _onExit;

    public:
        void setState(StateEnum newState)
        {
            if (_onExit)  _onExit(_currentState);
            _currentState = newState;
            if (_onEnter) _onEnter(_currentState);
        }

        StateEnum getState() const { return _currentState; }

        bool isState(StateEnum state) const { return _currentState == state; }

        void onStateEnter(std::function<void(StateEnum)> cb) { _onEnter = std::move(cb); }
        void onStateExit (std::function<void(StateEnum)> cb) { _onExit  = std::move(cb); }
    };

    // ── MultiCustomStateManager — one class, all enum overloads ──────────────
    template<typename... StateEnums>
    class MultiCustomStateManager : public SingleStateManager<StateEnums>...
    {
    public:
        using SingleStateManager<StateEnums>::setState...;
        using SingleStateManager<StateEnums>::getState...;
        using SingleStateManager<StateEnums>::isState...;
        using SingleStateManager<StateEnums>::onStateEnter...;
        using SingleStateManager<StateEnums>::onStateExit...;
    };

    // Empty specialization — no custom states
    template<>
    class MultiCustomStateManager<>
    {
    public:
        void setState(...)       {}
        void onStateEnter(...)   {}
        void onStateExit(...)    {}
    };

    // ── Extract State enum from a Manifest ────────────────────────────────────
    template<typename M, typename = void>
    struct extract_ManifestState { using type = void; };

    template<typename M>
    struct extract_ManifestState<M, std::void_t<typename M::State>>
    { using type = typename M::State; };

    // ── Void filtering (reuse same pattern as flags) ──────────────────────────
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

    // ── Gather all state enums from component manifest + trait manifests ──────
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

#endif //MULTICUSTOMSTATEMANAGER_H