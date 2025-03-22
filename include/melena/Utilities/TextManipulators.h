//
// Created by Dave R. Smith on 3/12/25.
//

#ifndef TEXTMANIPULATORS_H
#define TEXTMANIPULATORS_H

#include <string>

#include <iostream>
#include <sstream>
#include "SFML/Graphics/Text.hpp"
namespace ml
{
	class TextManipulators
	{

	public:
		static std::string wordwrap(const sf::Text &text, float maxwidth);
		static std::string wordwrap(const std::string &input, const sf::Font &font, unsigned int characterSize,
									float maxWidth);
	};


} // namespace ml
#endif // TEXTMANIPULATORS_H
