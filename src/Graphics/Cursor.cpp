
#include <malena/Graphics/Cursor.h>

ml::Cursor::Cursor(const sf::Font &font) : Shape(font), Text(font)
{
	Text::setString('|');
	enableState(Stateful::BLINKING);
	registerEvents();
}


void ml::Cursor::registerEvents()
{
	onUpdate([this] {
		if (checkState(Stateful::BLINKING))
		{
			if (clock.getElapsedTime().asMilliseconds() > 500)
			{
				toggleState(Stateful::HIDDEN);
				clock.restart();
			}
		}
	});
}
