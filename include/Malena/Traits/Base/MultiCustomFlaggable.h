#ifndef MULTICUSTOMFLAGGABLE_H
#define MULTICUSTOMFLAGGABLE_H

#include <unordered_map>
#include <tuple>
#include <type_traits>
#include <Malena/Utilities/EnumClassHash.h>

namespace ml
{
    // ── SingleFlaggable — one flag store per enum type ────────────────────────
    template<typename Enum>
    class SingleFlaggable
    {
        std::unordered_map<Enum, bool, EnumClassHash> _states;
    public:
        void enableFlag(Enum flag)          { _states[flag] = true; }
        void disableFlag(Enum flag)         { _states[flag] = false; }
        bool checkFlag(Enum flag) const     { auto it = _states.find(flag); return it != _states.end() && it->second; }
        void setFlag(Enum flag, bool value) { _states[flag] = value; }
        void toggleFlag(Enum flag)          { _states[flag] = !checkFlag(flag); }
    };

    // ── MultiCustomFlaggable — one class, all enum overloads ─────────────────
    template<typename... Enums>
    class MultiCustomFlaggable : public SingleFlaggable<Enums>...
    {
    public:
        using SingleFlaggable<Enums>::enableFlag...;
        using SingleFlaggable<Enums>::disableFlag...;
        using SingleFlaggable<Enums>::checkFlag...;
        using SingleFlaggable<Enums>::setFlag...;
        using SingleFlaggable<Enums>::toggleFlag...;
    };

    // Empty specialization — no custom flags, no-op methods so using declarations compile
    template<>
    class MultiCustomFlaggable<>
    {
    public:
        void enableFlag(...)  {}
        void disableFlag(...) {}
        bool checkFlag(...)  const { return false; }
        void setFlag(...)    {}
        void toggleFlag(...) {}
    };

    // ── Detect manifest_type on a Customizable subclass ───────────────────────
    template<typename T, typename = void>
    struct extract_TraitManifest { using type = void; };

    template<typename T>
    struct extract_TraitManifest<T, std::void_t<typename T::manifest_type>>
    { using type = typename T::manifest_type; };

    // ── Extract Flags enum from a Manifest ────────────────────────────────────
    template<typename M, typename = void>
    struct extract_ManifestFlags { using type = void; };

    template<typename M>
    struct extract_ManifestFlags<M, std::void_t<typename M::Flag>>
    { using type = typename M::Flag; };

    // ── Void filtering ────────────────────────────────────────────────────────
    template<typename T, typename Acc>
    struct AppendIfNotVoid { using type = Acc; };

    template<typename T, typename... Acc>
    struct AppendIfNotVoid<T, std::tuple<Acc...>>
    {
        using type = std::conditional_t<
            std::is_void_v<T>,
            std::tuple<Acc...>,
            std::tuple<Acc..., T>
        >;
    };

    template<typename... Ts>
    struct FilterVoid;

    template<>
    struct FilterVoid<> { using type = std::tuple<>; };

    template<typename T, typename... Rest>
    struct FilterVoid<T, Rest...>
    {
        using type = typename AppendIfNotVoid<
            T,
            typename FilterVoid<Rest...>::type
        >::type;
    };

    template<typename Tuple>
    struct TupleToMultiFlaggable;

    template<typename... Enums>
    struct TupleToMultiFlaggable<std::tuple<Enums...>>
    { using type = MultiCustomFlaggable<Enums...>; };

    // ── Gather all flag enums from component manifest + trait manifests ───────
    template<typename ComponentManifest, typename... Traits>
    struct GatherFlags
    {
        using AllFlagsTuple = typename FilterVoid<
            typename extract_ManifestFlags<ComponentManifest>::type,
            typename extract_ManifestFlags<
                typename extract_TraitManifest<Traits>::type
            >::type...
        >::type;

        using type = typename TupleToMultiFlaggable<AllFlagsTuple>::type;
    };

} // namespace ml

#endif //MULTICUSTOMFLAGGABLE_H