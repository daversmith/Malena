#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <Malena/Resources/ResourceManager.h>
#include "Malena/Manifests/DefaultManifest.h"
#include <SFML/Graphics/Font.hpp>
#include <Malena/Resources/Fonts/arial.h>
#include <Malena/Utilities/LoaderFunctions.h>

namespace ml
{
    /**
     * @brief Manifest-driven cache for @c sf::Font resources.
     *
     * @c FontManager<Manifest> extends @c ResourceManager to provide
     * lazy-loaded, cached access to fonts declared in a manifest's
     * @c Fonts enum. It also provides @c getDefault(), which returns a
     * built-in Arial font that is always available without any manifest
     * registration.
     *
     * @c FontManager defaults to @c DefaultManifest so it can be used
     * without a custom manifest when only the built-in font is needed:
     * @code
     * // Built-in Arial — no manifest required
     * const sf::Font& arial = ml::FontManager<>::getDefault();
     *
     * // Manifest-declared font
     * const sf::Font& main = ml::FontManager<MyManifest>::get(MyManifest::Fonts::Main);
     *
     * // Via AssetsManager (preferred when multiple asset types are needed)
     * const sf::Font& main = ml::AssetsManager<MyManifest>::get(MyManifest::Fonts::Main);
     * @endcode
     *
     * @tparam Manifest A @c Manifest subclass with a @c Fonts enum whose
     *                  values map to font file paths. Defaults to
     *                  @c DefaultManifest when no custom fonts are needed.
     *
     * @see ResourceManager, AssetsManager, DefaultManifest
     */
    template<typename Manifest = DefaultManifest>
    /**
     * @brief FontManager.
     * @ingroup Resources
     */
    class FontManager : public ResourceManager<Manifest, sf::Font, &fileOpener>
    {
    public:
        /**
         * @brief Return the built-in Arial font.
         *
         * The Arial font is embedded in the framework and requires no
         * manifest registration or file path. Use it as a reliable fallback
         * for UI text when no custom font has been loaded.
         *
         * @return Const reference to the built-in @c sf::Font.
         */
        static const sf::Font& getDefault();
    };

} // namespace ml

#include "../../../src/Resources/FontManager.tpp"
#endif // FONTMANAGER_H
