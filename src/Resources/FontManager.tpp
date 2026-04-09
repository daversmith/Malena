#include <Malena/Resources/FontManager.h>
#include <Malena/Resources/Fonts/arial.h>

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
	template<typename Manifest>
	inline void FontManager<Manifest>::prewarm(const sf::Font& font, unsigned int charSize)
	{
		sf::Text warmup(font, "", charSize);
		sf::String all;
		for (char32_t c = 32; c < 127; ++c)
			all += c;
		warmup.setString(all);
		static_cast<void>(warmup.getLocalBounds()); // forces ALL glyphs to load into atlas NOW
	}

} // namespace ml
#endif