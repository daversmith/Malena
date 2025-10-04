#ifndef FONTMANAGER_H
#define FONTMANAGER_H
#include <Malena/Managers/ResourceManager.h>

#include "Malena/Manifests/DefaultManifest.h"
#include <SFML/Graphics/Font.hpp>
#include <Malena/Managers/Fonts/arial.h>
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
#include "../../../src/Managers/FontManager.cpp"
#endif