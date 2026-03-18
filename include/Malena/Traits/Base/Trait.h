//
// Created by Dave Smith on 3/11/26.
//

#ifndef TRAIT_H
#define TRAIT_H

#include <Malena/Traits/Customizable.h>

namespace ml
{
    /**
     * @brief Empty marker base class for all Malena traits.
      * @ingroup TraitsBase
     *
     * @c Trait serves as the common root of the trait hierarchy. It carries
     * no data or virtual methods — its purpose is to give every trait a
     * shared, identifiable base type that template machinery (e.g.,
     * @c GatherFlags, @c HasCoreTraits) can detect at compile time.
     *
     * Traits that need no manifest derive from @c Trait directly:
     * @code
     * class Subscribable : public Trait { ... };
     * class Flaggable    : public Trait { ... };
     * @endcode
     *
     * Traits that declare their own manifest use @c TraitWith instead.
     *
     * @see TraitWith, Customizable, Draggable
     */
    class Trait {};

    /**
     * @brief Base for traits that declare a manifest.
     *
     * @c TraitWith<Manifest> combines the @c Trait marker with
     * @c Customizable<Manifest>, wiring the trait's @c Flags and @c State
     * enums into the @c GatherFlags / @c GatherStates machinery so they are
     * automatically included in the flag and state stores of any
     * @c ComponentWith that mixes in this trait.
     *
     * ### Usage
     * @code
     * class MyTraitManifest : public Manifest {
     * public:
     *     enum class Flag  { Active };
     *     enum class State { Idle, Running };
     * };
     *
     * class MyTrait : public ml::TraitWith<MyTraitManifest> {
     *     // access flags via dynamic_cast<SingleFlaggable<MyTraitManifest::Flag>*>(this)
     * };
     * @endcode
     *
     * @tparam Manifest A @c Manifest subclass declaring @c Flag/@c State enums.
     *
     * @see Trait, Customizable, Draggable, GatherFlags, GatherStates
     */
    template<typename Manifest>
    class TraitWith : public Trait,
                      public ml::Customizable<Manifest>
    {};

} // namespace ml

#endif // TRAIT_H
