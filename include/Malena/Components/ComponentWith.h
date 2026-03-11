// //
// // Created by Dave Smith on 3/8/26.
// //
//
// #ifndef COMPONENTWITH_H
// #define COMPONENTWITH_H
//
#include <Malena/Components/Component.h>
//
// namespace ml
// {
//     /**
//      * @brief Component with a manifest and optional traits.
//      *
//      * Automatically gathers flag enums from the component manifest
//      * and all trait manifests into one MultiCustomFlaggable.
//      * All three flag types work with zero ambiguity:
//      *
//      * @code
//      * class Carousel : public ml::ComponentWith<CarouselManifest, Draggable> {};
//      *
//      * _carousel.enableFlag(ml::Flag::DRAGGABLE);                  // system flag
//      * _carousel.enableFlag(CarouselManifest::Flags::SCROLLING);   // component flag
//      * _carousel.enableFlag(DraggableManifest::Flags::DRAGGING);   // trait flag
//      * @endcode
//      *
//      * Traits expose their manifest via Customizable:
//      * @code
//      * class Draggable : public ml::Customizable<DraggableManifest> {};
//      * @endcode
//      *
//      * Traits access their own flags via dynamic_cast:
//      * @code
//      * auto* f = dynamic_cast<ml::SingleFlaggable<DraggableManifest::Flags>*>(this);
//      * f->enableFlag(DraggableManifest::Flags::DRAGGING);
//      * @endcode
//      *
//      * @tparam Manifest A class with optional Flags and State enums
//      * @tparam Traits   Optional opt-in traits
//      */
//     template<typename Manifest, typename... Traits>
//     class ComponentWith : public ComponentBase<Manifest, Traits...> {};
//
// } // namespace ml
//
// #endif //COMPONENTWITH_H