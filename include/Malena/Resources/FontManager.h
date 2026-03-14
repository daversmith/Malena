#ifndef FONTMANAGER_H
#define FONTMANAGER_H
#include <Malena/Resources/ResourceManager.h>

#include "Malena/Manifests/DefaultManifest.h"
#include <SFML/Graphics/Font.hpp>
#include <Malena/Resources/Fonts/arial.h>
#include <Malena/Utilities/LoaderFunctions.h>
namespace ml
{

	template<typename Manifest = DefaultManifest>
class FontManager : public ResourceManager<Manifest, sf::Font, &fileOpener>
	{
	public:
		static const sf::Font& getDefault();
	};
}
#include "../../../src/Resources/FontManager.tpp"
#endif
