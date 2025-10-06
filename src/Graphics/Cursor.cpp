
#include <Malena/Graphics/Cursor.h>

ml::Cursor::Cursor(const sf::Font &font) : Shape(font), ml::Text(font)
{
	Text::setString('|');
	enableFlag(State::BLINKING);
	registerEvents();
}


void ml::Cursor::registerEvents()
{
	onUpdate([this] {
		if (checkFlag(State::BLINKING))
		{
			if (clock.getElapsedTime().asMilliseconds() > 500)
			{
				toggleFlag(State::HIDDEN);
				clock.restart();
			}
		}
	});
}
