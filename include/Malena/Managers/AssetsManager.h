//
// Created by Dave Smith on 10/3/25.
//

#ifndef ASSETSMANAGER_H
#define ASSETSMANAGER_H
#include <Malena/Managers/FontManager.h>
#include <Malena/Managers/TextureManager.h>
#include <Malena/Managers/SoundManager.h>
#include <Malena/Managers/ConfigManager.h>

namespace ml {
    // Helper traits to detect if enum types exist
    template<typename T, typename = void>
    struct has_Image : std::false_type {};

    template<typename T>
    struct has_Image<T, std::void_t<typename T::Images>> : std::true_type {};

    template<typename T, typename = void>
    struct has_Font : std::false_type {};

    template<typename T>
    struct has_Font<T, std::void_t<typename T::Fonts>> : std::true_type {};

    template<typename T, typename = void>
    struct has_Sound : std::false_type {};

    template<typename T>
    struct has_Sound<T, std::void_t<typename T::Sounds>> : std::true_type {};

    template<typename Manifest>
    class AssetsManager {
    private:
        using TextureMgr = TextureManager<Manifest>;
        using FontMgr = FontManager<Manifest>;
        using SoundMgr = SoundManager<Manifest>;
        // using Config = ConfigManager<Manifest>;

    public:
        // Only define get() for Image if Manifest has Image enum
        template<typename M = Manifest>
        static std::enable_if_t<has_Image<M>::value, const sf::Texture&>
        get(typename M::Images image) {
            return TextureMgr::get(image);
        }

        // Only define get() for Font if Manifest has Font enum
        template<typename M = Manifest>
        static std::enable_if_t<has_Font<M>::value, const sf::Font&>
        get(typename M::Fonts font) {
            return FontMgr::get(font);
        }

        // Only define get() for Sound if Manifest has Sound enum
        template<typename M = Manifest>
        static std::enable_if_t<has_Sound<M>::value, const sf::SoundBuffer&>
        get(typename M::Sounds sound) {
            return SoundMgr::get(sound);
        }

        // getDefaultFont only available if Manifest has Font enum
        template<typename M = Manifest>
        static std::enable_if_t<has_Font<M>::value, const sf::Font&>
        getDefaultFont() {
            return FontMgr::getDefault();
        }
    };
}
#endif //ASSETSMANAGER_H
