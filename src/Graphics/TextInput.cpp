
#include <Malena/Graphics/TextInput.h>

#include <Malena/Traits/Flaggable.h>

namespace ml
{
	TextInput::TextInput()
	{
		cursor.enableState(State::BLINKING);
		enableState(State::FOCUSED);
	}

	void TextInput::draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		typer.draw(target, states);
		if (!cursor.checkState(ml::State::HIDDEN))
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

	void TextInput::enableState(const State state)
	{
		Stateful::enableState(state);
		typer.enableState(state);
		cursor.enableState(state);
	}
} // namespace ml
