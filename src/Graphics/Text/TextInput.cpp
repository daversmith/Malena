
#include <Malena/Graphics/Text/TextInput.h>

#include <Malena/Resources/FlagManager.h>

namespace ml
{
	TextInput::TextInput()
	{
		cursor.enableFlag(ml::Flag::BLINKING);
		// enableState(ml::Flag::FOCUSED);
	}

	void TextInput::draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		typer.draw(target, states);
		if (!cursor.checkFlag(ml::Flag::HIDDEN))
			cursor.draw(target, states);
	}

	void TextInput::setPosition(const sf::Vector2f &position)
	{
	}

	sf::Vector2f TextInput::getPosition() const
	{
		return cursor.getPosition();
	}

	sf::FloatRect TextInput::getGlobalBounds() const
	{
		return cursor.getGlobalBounds();
	}

	// void TextInput::enableState(const ml::State state)
	// {
	// 	Flaggable::enableFlag(state);
	// 	typer.enableFlag(state);
	// 	cursor.enableFlag(state);
	// }
} // namespace ml
