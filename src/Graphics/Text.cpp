#ifndef TEXT_CPP
#define TEXT_CPP
#include <Malena/Graphics/Text.h>

#include <Malena/Utilities/TextManipulators.h>

namespace ml
{
	template<typename Manifest>
	TextWith<Manifest>::TextWith(const sf::Font &font) : Shape<sf::Text, Manifest>(font)
	{
	}

	template<typename Manifest>
	void TextWith<Manifest>::setWordWrap(bool word_wrap)
	{
		wordWrap = word_wrap;
	}

	template<typename Manifest>
	void TextWith<Manifest>::setMaxWidth(float max_width)
	{
		maxWidth = max_width;
	}

	template<typename Manifest>
	void TextWith<Manifest>::setString(const sf::String &text)
	{
		if (wordWrap)
			TextManipulators::wordwrap(*this, maxWidth);
		else
			sf::Text::setString(text);
	}
} // namespace ml
#endif