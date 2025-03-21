
#include "Cursor.h"

ml::Cursor::Cursor(const sf::Font& font) : Shape(font), Text(font)
{
    Text::setString('|');
    enableState(Stateful::BLINKING);
}

void ml::Cursor::eventHandler(const std::optional<sf::Event> &event)
{
    Text::eventHandler(event);
}

void ml::Cursor::update()
{
    Text::update();
    if(checkState(Stateful::BLINKING))
    {
        if(clock.getElapsedTime().asMilliseconds() > 500)
        {
            toggleState(Stateful::HIDDEN);
            clock.restart();
        }
    }
}
