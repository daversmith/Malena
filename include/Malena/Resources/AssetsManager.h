// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 10/3/25.
//

#ifndef ASSETSMANAGER_H
#define ASSETSMANAGER_H

#include <Malena/Resources/FontManager.h>
#include <Malena/Resources/TextureManager.h>
#include <Malena/Resources/SoundManager.h>
#include <Malena/Resources/ConfigManager.h>

namespace ml
{
    /// @cond INTERNAL

    /** @brief Detects whether @c T declares an @c Images enum. */
    template<typename T, typename = void>
    struct has_Image : std::false_type {};
    template<typename T>
    struct has_Image<T, std::void_t<typename T::Images>> : std::true_type {};

    /** @brief Detects whether @c T declares a @c Fonts enum. */
    template<typename T, typename = void>
    struct has_Font : std::false_type {};
    template<typename T>
    struct has_Font<T, std::void_t<typename T::Fonts>> : std::true_type {};

    /** @brief Detects whether @c T declares a @c Sounds enum. */
    template<typename T, typename = void>
    struct has_Sound : std::false_type {};
    template<typename T>
    struct has_Sound<T, std::void_t<typename T::Sounds>> : std::true_type {};

    /// @endcond

    /**
     * @brief Unified asset accessor for textures, fonts, and sound buffers.
     *
     * @c AssetsManager<Manifest> combines @c TextureManager, @c FontManager,
     * and @c SoundManager into a single class with overloaded @c get() methods.
     * Each overload is enabled only when the manifest declares the corresponding
     * enum (@c Images, @c Fonts, or @c Sounds), so attempting to retrieve an
     * asset type the manifest does not declare is a compile error rather than a
     * runtime failure.
     *
     * ### When to use @c AssetsManager vs individual managers
     *
     * | Scenario | Recommended |
     * |----------|-------------|
     * | Only textures needed | @c TextureManager<Manifest> |
     * | Only fonts needed | @c FontManager<Manifest> |
     * | Mix of textures, fonts, sounds | @c AssetsManager<Manifest> |
     * | Also need config values | @c Context<Manifest> |
     *
     * ### Usage
     * @code
     * class MyManifest : public ml::Manifest {
     * public:
     *     enum class Images { Background, Player };
     *     enum class Fonts  { Main };
     *     enum class Sounds { Jump };
     * private:
     *     inline static const auto _ = [](){
     *         set(Images::Background, "assets/bg.png");
     *         set(Images::Player,     "assets/player.png");
     *         set(Fonts::Main,        "assets/main.ttf");
     *         set(Sounds::Jump,       "assets/jump.wav");
     *         return 0;
     *     }();
     * };
     *
     * // All three asset types via one manager
     * const sf::Texture&     bg  = ml::AssetsManager<MyManifest>::get(MyManifest::Images::Background);
     * const sf::Font&        fnt = ml::AssetsManager<MyManifest>::get(MyManifest::Fonts::Main);
     * const sf::SoundBuffer& sfx = ml::AssetsManager<MyManifest>::get(MyManifest::Sounds::Jump);
     *
     * // Default font (built-in Arial) — no manifest entry needed
     * const sf::Font& arial = ml::AssetsManager<MyManifest>::getDefaultFont();
     * @endcode
     *
     * @tparam Manifest A @c Manifest subclass declaring @c Images, @c Fonts,
     *                  and/or @c Sounds enums.
     *
     * @see TextureManager, FontManager, SoundManager, Context, Manifest
     */
    template<typename Manifest>
    /**
     * @brief AssetsManager.
     * @ingroup Resources
     */
    class AssetsManager
    {
    private:
        using TextureMgr = TextureManager<Manifest>;
        using FontMgr    = FontManager<Manifest>;
        using SoundMgr   = SoundManager<Manifest>;

    public:
        /**
         * @brief Retrieve a cached @c sf::Texture by manifest enum value.
         *
         * Only available when @c Manifest declares an @c Images enum.
         *
         * @param image The @c Images enum value identifying the texture.
         * @return Const reference to the cached @c sf::Texture.
         */
        template<typename M = Manifest>
        static std::enable_if_t<has_Image<M>::value, const sf::Texture&>
        get(typename M::Images image);

        /**
         * @brief Retrieve a cached @c sf::Font by manifest enum value.
         *
         * Only available when @c Manifest declares a @c Fonts enum.
         *
         * @param font The @c Fonts enum value identifying the font.
         * @return Const reference to the cached @c sf::Font.
         */
        template<typename M = Manifest>
        static std::enable_if_t<has_Font<M>::value, const sf::Font&>
        get(typename M::Fonts font);

        /**
         * @brief Retrieve a cached @c sf::SoundBuffer by manifest enum value.
         *
         * Only available when @c Manifest declares a @c Sounds enum.
         *
         * @param sound The @c Sounds enum value identifying the sound buffer.
         * @return Const reference to the cached @c sf::SoundBuffer.
         */
        template<typename M = Manifest>
        static std::enable_if_t<has_Sound<M>::value, const sf::SoundBuffer&>
        get(typename M::Sounds sound);

        /**
         * @brief Return the built-in Arial font.
         *
         * Only available when @c Manifest declares a @c Fonts enum.
         * Delegates to @c FontManager::getDefault().
         *
         * @return Const reference to the built-in @c sf::Font.
         */
        template<typename M = Manifest>
        static std::enable_if_t<has_Font<M>::value, const sf::Font&>
        getDefaultFont();
    };

} // namespace ml

#include "../../../src/Resources/AssetsManager.tpp"
#endif // ASSETSMANAGER_H
