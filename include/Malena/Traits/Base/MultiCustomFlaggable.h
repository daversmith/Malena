// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_MULTICUSTOMFLAGGABLE_H
#define MALENA_MULTICUSTOMFLAGGABLE_H

#include <Malena/Core/malena_export.h>
#include <unordered_map>
#include <tuple>
#include <type_traits>
#include <Malena/Utilities/EnumClassHash.h>
#include <Malena/Utilities/TypeExtraction.h>

namespace ml
{
    // =========================================================================
    // SingleFlaggable
    // =========================================================================

    /**
     * @brief Flag store for a single enum type.
     *
     * Maintains an @c unordered_map from @c Enum values to @c bool and
     * exposes the standard five-method flag API. One @c SingleFlaggable
     * base is inherited per distinct flag enum.
     *
     * @note This class is an implementation detail of @c MultiCustomFlaggable.
     *       User code accesses flags through the component's unified API or,
     *       for trait-internal use, through a @c dynamic_cast to
     *       @c SingleFlaggable<TraitManifest::Flag>.
     *
     * @tparam Enum An @c enum @c class type used as flag keys.
     *
     * @see MultiCustomFlaggable, GatherFlags
     */
    template<typename Enum>
    class SingleFlaggable
    {
        std::unordered_map<Enum, bool, EnumClassHash> _states;

    public:
        /// Set @p flag to @c true.
        void enableFlag(Enum flag)          { _states[flag] = true; }

        /// Set @p flag to @c false.
        void disableFlag(Enum flag)         { _states[flag] = false; }

        /// Return @c true if @p flag is set, @c false if unset or never written.
        bool checkFlag(Enum flag) const     { auto it = _states.find(flag); return it != _states.end() && it->second; }

        /// Set @p flag to an explicit @p value.
        void setFlag(Enum flag, bool value) { _states[flag] = value; }

        /// Flip @p flag between @c true and @c false.
        void toggleFlag(Enum flag)          { _states[flag] = !checkFlag(flag); }
    };

    // =========================================================================
    // MultiCustomFlaggable
    // =========================================================================

    /**
     * @brief Aggregates flag stores for multiple enum types into one class.
     *
     * @c MultiCustomFlaggable<Enums...> inherits one @c SingleFlaggable per
     * enum in the pack and lifts all their methods into a single overload set
     * via @c using declarations. This allows a component to call a single
     * @c checkFlag overloaded on enum type rather than casting to the right base.
     *
     * It is instantiated by @c GatherFlags, which collects all @c Flag enums
     * from a component's manifest and from every trait mixed in via
     * @c ComponentWith. The result is a single base class that holds all
     * custom flag storage for the component.
     *
     * ### Example (automatic — no user action required)
     * @code
     * // Given:
     * struct MyManifest { enum class Flag { Selected }; };
     * class Draggable : public TraitWith<DraggableManifest> {};  // Flag::DRAGGING
     *
     * class MyWidget : public ml::ComponentWith<MyManifest, Draggable> {};
     *
     * MyWidget w;
     * w.enableFlag(MyManifest::Flag::Selected);          // stored in SingleFlaggable<MyManifest::Flag>
     * w.enableFlag(DraggableManifest::Flag::DRAGGING);   // stored in SingleFlaggable<DraggableManifest::Flag>
     * @endcode
     *
     * @tparam Enums Zero or more @c enum @c class types, one per flag set.
     *               The empty specialization (@c MultiCustomFlaggable<>) is
     *               used when no custom flags are declared.
     *
     * @ingroup TraitsBase
     * @see SingleFlaggable, GatherFlags, ComponentCore
     */
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

    /**
     * @brief No-op specialization used when no custom flags are declared.
     *
     * Inheriting from this empty specialization adds no storage and no
     * meaningful methods. The variadic @c ... parameters absorb any
     * accidental call so that @c using declarations in @c ComponentCore
     * still compile cleanly.
     */
    template<>
    class MultiCustomFlaggable<>
    {
    public:
        void enableFlag(...)         {}
        void disableFlag(...)        {}
        bool checkFlag(...)  const   { return false; }
        void setFlag(...)            {}
        void toggleFlag(...)         {}
    };

    // =========================================================================
    // Trait manifest detection helpers
    // =========================================================================

    /// @cond INTERNAL

    /**
     * @brief Extracts the @c Flag enum from a manifest struct.
     *
     * Yields @c void if the manifest has no @c Flag member type.
     */
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

    /// @endcond

    // =========================================================================
    // GatherFlags
    // =========================================================================

    /**
     * @brief Collects all @c Flag enums from a component manifest and its traits.
     *
     * @c GatherFlags<ComponentManifest, Traits...>::type resolves to a
     * @c MultiCustomFlaggable instantiation whose template parameters are
     * exactly the non-void @c Flag enums found in:
     * - @c ComponentManifest::Flag (if it exists)
     * - @c TraitManifest::Flag for each @c Trait that has a @c manifest_type
     *
     * @c ComponentCore inherits this type to get a single, unified flag store
     * for the entire component hierarchy. @c void entries are filtered out
     * automatically, so manifests or traits without flags add no overhead.
     *
     * @tparam ComponentManifest The component's own manifest, or @c void.
     * @tparam Traits            The extra traits mixed into the component.
     *
     * @see MultiCustomFlaggable, ComponentCore, GatherStates
     */
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

#endif // MALENA_MULTICUSTOMFLAGGABLE_H
