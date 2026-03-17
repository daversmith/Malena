//
// Created by Dave Smith on 3/7/26.
//

#ifndef MANIFESTALIASES_H
#define MANIFESTALIASES_H

namespace ml
{
    /// @cond INTERNAL

    /**
     * @brief Conditionally exposes @c Images type alias from a manifest.
     * Empty when the manifest has no @c Images enum.
     */
    template<typename Manifest, typename = void>
    struct ExtractImages {};

    template<typename Manifest>
    struct ExtractImages<Manifest, std::void_t<typename Manifest::Images>>
    {
        using Images = typename Manifest::Images;
    };

    /**
     * @brief Conditionally exposes @c Fonts type alias from a manifest.
     * Empty when the manifest has no @c Fonts enum.
     */
    template<typename Manifest, typename = void>
    struct ExtractFonts {};

    template<typename Manifest>
    struct ExtractFonts<Manifest, std::void_t<typename Manifest::Fonts>>
    {
        using Fonts = typename Manifest::Fonts;
    };

    /**
     * @brief Conditionally exposes @c Sounds type alias from a manifest.
     * Empty when the manifest has no @c Sounds enum.
     */
    template<typename Manifest, typename = void>
    struct ExtractSounds {};

    template<typename Manifest>
    struct ExtractSounds<Manifest, std::void_t<typename Manifest::Sounds>>
    {
        using Sounds = typename Manifest::Sounds;
    };

    /**
     * @brief Conditionally exposes @c State type alias from a manifest.
     * Empty when the manifest has no @c State enum.
     */
    template<typename Manifest, typename = void>
    struct ExtractState {};

    template<typename Manifest>
    struct ExtractState<Manifest, std::void_t<typename Manifest::State>>
    {
        using State = typename Manifest::State;
    };

    /**
     * @brief Conditionally exposes @c Flags type alias from a manifest.
     * Empty when the manifest has no @c Flags enum.
     */
    template<typename Manifest, typename = void>
    struct ExtractFlags {};

    template<typename Manifest>
    struct ExtractFlags<Manifest, std::void_t<typename Manifest::Flags>>
    {
        using Flags = typename Manifest::Flags;
    };

    /// @endcond

    /**
     * @brief Pulls manifest inner type aliases into a class's scope.
     *
     * @c ManifestAliases<Manifest> inherits from the five @c Extract*
     * helpers above, each of which conditionally introduces one @c using
     * alias. Only the aliases whose corresponding enum exists in @c Manifest
     * are injected — missing enums simply produce an empty base class with
     * no ambiguity or compile error.
     *
     * The result is that any class inheriting @c ManifestAliases can write
     * @c Images::Background, @c Fonts::Main, etc. directly, without fully
     * qualifying the manifest type every time.
     *
     * ### Which aliases are injected
     *
     * | Manifest member | Alias introduced |
     * |-----------------|-----------------|
     * | @c Manifest::Images | @c Images |
     * | @c Manifest::Fonts  | @c Fonts  |
     * | @c Manifest::Sounds | @c Sounds |
     * | @c Manifest::State  | @c State  |
     * | @c Manifest::Flags  | @c Flags  |
     *
     * ### Usage
     * @c ManifestAliases is inherited automatically through @c Context and
     * @c Resources. Direct use is uncommon outside of framework internals.
     * @code
     * struct MyContext : public ml::ManifestAliases<MyManifest> {};
     *
     * // Now usable unqualified:
     * MyContext::Images::Background
     * MyContext::Fonts::Main
     * @endcode
     *
     * @tparam Manifest A manifest struct, typically a @c Manifest subclass.
     *
     * @see Context, Resources, Manifest
     */
    template<typename Manifest>
    struct ManifestAliases :
        ExtractImages<Manifest>,
        ExtractFonts<Manifest>,
        ExtractSounds<Manifest>,
        ExtractState<Manifest>,
        ExtractFlags<Manifest>
    {};

} // namespace ml

#endif // MANIFESTALIASES_H
