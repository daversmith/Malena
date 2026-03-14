
#include <Malena/Graphics/Text/Cursor.h>

ml::Cursor::Cursor(const sf::Font &font) : Graphic(font), ml::Text(font)
{
	Text::setString('|');
	//enableFlag(Flag::BLINKING);
	registerEvents();
}


void ml::Cursor::registerEvents()
{
	onUpdate([this] {
		// if (checkFlag(State::BLINKING))
		// {
		// 	if (clock.getElapsedTime().asMilliseconds() > 500)
		// 	{
		// 		toggleFlag(State::HIDDEN);
		// 		clock.restart();
		// 	}
		// }
	});
}
