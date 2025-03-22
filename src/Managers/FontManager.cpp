//
// Created by Dave R. Smith on 3/9/25.
//

#include <melena/Managers/FontManager.h>

namespace ml
{
	const sf::Font &FontManager::DEFAULT = FontManager::getFont(FontManager::ARIAL);

	const sf::Font &FontManager::getFont(const Font font)
	{
		if (fonts.count(font) < 1)
			loadFont(font);
		return fonts.at(font);
	}

	void FontManager::loadFont(Font font)
	{
		fonts[font] = sf::Font();
		fonts.at(font).openFromFile(getPath(font));
	}

	std::string FontManager::getPath(Font font)
	{
		switch (font)
		{
			case ARIAL:
				return "fonts/arial.ttf";
			default:
				return "fonts/arial.ttf";
		}
	}
} // namespace ml
