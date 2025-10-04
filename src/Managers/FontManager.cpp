#include <Malena/Managers/FontManager.h>
#include <Malena/Managers/Fonts/arial.h>

#include <cstring>
#include <fstream>
#include <algorithm>
#ifndef FONTMANAGER_CPP
#define FONTMANAGER_CPP
namespace ml
{
	template<typename Manifest>
	const sf::Font& FontManager<Manifest>::getDefault() {
		static sf::Font arialFont = []() {
			sf::Font font;
			if (!font.openFromMemory(fonts::arial_data.data(), fonts::arial_data_size)) {
				throw std::runtime_error("Failed to load built-in Arial font");
			}
			return font;
		}();
		return arialFont;
	}

} // namespace ml
#endif