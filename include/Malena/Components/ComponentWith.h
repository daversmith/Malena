//
// Created by Dave Smith on 3/8/26.
//

#ifndef COMPONENTWITH_H
#define COMPONENTWITH_H

#include <Malena/Components/Component.h>
#include <Malena/Traits/Customizable.h>

namespace ml
{
    /**
     * @brief Alias for Customizable<Manifest, Component<Traits...>>.
     *
     * System flags (ml::Flag) and custom manifest flags both work
     * from a single call site with no ambiguity or using declarations.
     *
     * @code
     * class Carousel : public ml::ComponentWith<CarouselManifest> {};
     * class Carousel : public ml::ComponentWith<CarouselManifest, Draggable> {};
     *
     * _carousel.enableFlag(ml::Flag::DRAGGABLE);              // system flag
     * _carousel.enableFlag(CarouselManifest::Flags::LOOPING); // custom flag
     * @endcode
     *
     * @tparam Manifest A class inheriting ml::Manifest
     * @tparam Traits   Optional opt-in traits (e.g. Draggable, Messenger)
     */
    template<typename Manifest, typename... Traits>
    using ComponentWith = Customizable<Manifest, Component<Traits...>>;

} // namespace ml

#endif //COMPONENTWITH_H