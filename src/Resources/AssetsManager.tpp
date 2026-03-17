//
// Created by Dave Smith on 3/6/26.
//

#ifndef ASSESTS_MANAGER_CPP
#define ASSESTS_MANAGER_CPP
#include <Malena/Resources/AssetsManager.h>

namespace ml
{
    // Only define get() for Image if Manifest has Image enum
    template<typename Manifest>
    template<typename M>
    std::enable_if_t<has_Image<M>::value, const sf::Texture&>
    AssetsManager<Manifest>::get(typename M::Images image) {
        return TextureMgr::get(image);
    }

    // Only define get() for Font if Manifest has Font enum
    template<typename Manifest>
    template<typename M>
    std::enable_if_t<has_Font<M>::value, const sf::Font&>
    AssetsManager<Manifest>::get(typename M::Fonts font) {
        return FontMgr::get(font);
    }

    // Only define get() for Sound if Manifest has Sound enum
    template<typename Manifest>
    template<typename M>
    std::enable_if_t<has_Sound<M>::value, const sf::SoundBuffer&>
    AssetsManager<Manifest>::get(typename M::Sounds sound) {
        return SoundMgr::get(sound);
    }

    // getDefaultFont only available if Manifest has Font enum
    template<typename Manifest>
    template<typename M>
    std::enable_if_t<has_Font<M>::value, const sf::Font&>
    AssetsManager<Manifest>::getDefaultFont() {
        return FontMgr::getDefault();
    }
}// namespace ml
#endif