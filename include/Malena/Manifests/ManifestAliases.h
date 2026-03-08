//
// Created by Dave Smith on 3/7/26.
//

#ifndef MANIFESTALIASES_H
#define MANIFESTALIASES_H
namespace ml {

    // Individual conditional bases
    template<typename Manifest, typename = void>
    struct ExtractImages {};

    template<typename Manifest>
    struct ExtractImages<Manifest, std::void_t<typename Manifest::Images>> {
        using Images = typename Manifest::Images;
    };

    // ---

    template<typename Manifest, typename = void>
    struct ExtractFonts {};

    template<typename Manifest>
    struct ExtractFonts<Manifest, std::void_t<typename Manifest::Fonts>> {
        using Fonts = typename Manifest::Fonts;
    };

    // ---

    template<typename Manifest, typename = void>
    struct ExtractSounds {};

    template<typename Manifest>
    struct ExtractSounds<Manifest, std::void_t<typename Manifest::Sounds>> {
        using Sounds = typename Manifest::Sounds;
    };

    // ---

    template<typename Manifest, typename = void>
    struct ExtractState {};

    template<typename Manifest>
    struct ExtractState<Manifest, std::void_t<typename Manifest::State>> {
        using State = typename Manifest::State;
    };

    // ---

    template<typename Manifest, typename = void>
    struct ExtractFlags {};

    template<typename Manifest>
    struct ExtractFlags<Manifest, std::void_t<typename Manifest::Flags>> {
        using Flags = typename Manifest::Flags;
    };

    // --- Single base to inherit from ---
    template<typename Manifest>
    struct ManifestAliases :
        ExtractImages<Manifest>,
        ExtractFonts<Manifest>,
        ExtractSounds<Manifest>,
        ExtractState<Manifest>,
        ExtractFlags<Manifest>
    {};

} // namespace ml
#endif //MANIFESTALIASES_H
