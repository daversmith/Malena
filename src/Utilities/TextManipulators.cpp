//
// Created by Dave R. Smith on 3/12/25.
//

#include <Malena/Utilities/TextManipulators.h>

namespace ml
{
	std::string TextManipulators::wordwrap(const sf::Text &text, float maxwidth)
	{
		return wordwrap(text.getString(), text.getFont(), text.getCharacterSize(), maxwidth);
	}

	std::string TextManipulators::wordwrap(const std::string &input, const sf::Font &font,
									   const unsigned int characterSize, const float maxWidth)
	{
		sf::Text text(font);
		text.setCharacterSize(characterSize);

		auto lineWidth = [&](const std::string &s) -> float {
			text.setString(s);
			const auto b = text.getLocalBounds();
			return b.position.x + b.size.x;   // include left bearing
		};

		std::string result, line;
		std::istringstream iss(input);
		std::string word;

		while (iss >> word)
		{
			std::string testLine = line.empty() ? word : line + " " + word;

			if (lineWidth(testLine) > maxWidth && !line.empty())
			{
				result += line + "\n";
				line = word;            // word may itself overflow; handled next pass / at flush
			}
			else
			{
				line = testLine;
			}
		}

		if (!line.empty())
			result += line;

		return result;
	}
} // namespace ml
