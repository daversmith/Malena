
#include <Malena/Graphics/Text/Text.h>
#include <Malena/Utilities/Utf8.h>

#include <Malena/Utilities/TextManipulators.h>

namespace ml
{
	Text::Text(const sf::Font &font) : Graphic<sf::Text>(font)
	{
	}


	void Text::setWordWrap(bool word_wrap)
	{
		wordWrap = word_wrap;
		this->setString(this->getString());
	}


	void Text::setMaxWidth(float max_width)
	{
		maxWidth = max_width;
	}


	void Text::setString(const sf::String &text)
	{
		sf::Text::setString(text);
		if (wordWrap)
			sf::Text::setString(ml::utf8(TextManipulators::wordwrap(*this, maxWidth)));

	}

	// UTF-8 aware entry points — see the note in the header. Without these, the
	// implicit sf::String conversion decodes byte-by-byte via the locale and any
	// non-ASCII character renders as a run of tofu boxes.
	void Text::setString(const std::string& bytes)
	{
		setString(ml::utf8(bytes));
	}

	void Text::setString(const char* bytes)
	{
		setString(ml::utf8(bytes));
	}
} // namespace ml
