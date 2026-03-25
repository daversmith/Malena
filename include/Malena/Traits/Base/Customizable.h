// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef UICOMPONENT_H
#define UICOMPONENT_H

#include <Malena/Resources/StateManager.h>
#include <Malena/Utilities/TypeExtraction.h>

namespace ml
{
    /**
     * @brief Tags a class with a manifest type and wires in its @c State manager.
     *
     * @c Customizable<Manifest> is the base class for any framework type that
     * needs to declare a manifest — traits, graphics components, and plugins
     * all inherit from it directly or indirectly through the @c With<Manifest>
     * pattern.
     *
     * ### What @c Customizable does
     * - Exposes @c manifest_type so that @c GatherFlags and @c GatherStates
     *   can find this class's manifest when building the multi-flag and
     *   multi-state inheritance chains in @c ComponentCore.
     * - Inherits @c StateManager<State> where @c State is extracted from the
     *   manifest via @c extract_State<Manifest>::type. If the manifest has no
     *   @c State enum, @c extract_State yields @c void and @c StateManager
     *   becomes a no-op.
     *
     * ### What @c Customizable does NOT do
     * @c Customizable does @b not own @c CustomFlaggable. Flag storage is
     * managed by @c MultiCustomFlaggable inside @c ComponentBase, which
     * gathers flags from the component manifest @e and from every trait
     * that also declares a @c Flags enum. This prevents duplicate base
     * classes and ensures a single flag map per enum type.
     *
     * Traits that need to read or write their own flags do so by casting to
     * the appropriate @c SingleFlaggable base:
     * @code
     * auto* f = dynamic_cast<SingleFlaggable<DraggableManifest::Flags>*>(this);
     * if (f) f->enableFlag(DraggableManifest::Flags::DRAGGING);
     * @endcode
     *
     * ### Usage
     * @code
     * // A trait with its own manifest
     * class Draggable : public ml::Customizable<DraggableManifest> {};
     *
     * // A component with a manifest and an extra trait
     * class Carousel : public ml::ComponentWith<CarouselManifest, Draggable> {};
     * @endcode
     *
     * @tparam Manifest A struct with optional @c Flags and @c State enums
     *                  and optional resource list inner types.
     *
     * @see ComponentCore, GatherFlags, GatherStates, StateManager, MultiCustomFlaggable
     */
    template<typename Manifest>
    /**
     * @brief Customizable.
     * @ingroup Traits
     */
    class Customizable : public StateManager<typename extract_State<Manifest>::type>
    {
    public:
        /// @brief The manifest type, used by @c GatherFlags and @c GatherStates
        ///        to locate this class's flag and state declarations.
        using manifest_type = Manifest;

        Customizable() = default;
        virtual ~Customizable() = default;
    };

} // namespace ml

#endif // UICOMPONENT_H
