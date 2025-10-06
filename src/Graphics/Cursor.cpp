
#include <Malena/Graphics/Cursor.h>

ml::Cursor::Cursor(const sf::Font &font) : Shape(font), ml::Text(font)
{
	Text::setString('|');
	enableState(State::BLINKING);
	registerEvents();
}


void ml::Cursor::registerEvents()
{
	onUpdate([this] {
		if (checkState(State::BLINKING))
		{
			if (clock.getElapsedTime().asMilliseconds() > 500)
			{
				toggleState(State::HIDDEN);
				clock.restart();
			}
		}
	});
}
