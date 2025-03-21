
#include "TextInput.h"

namespace ml
{
    TextInput::TextInput()
    {
        cursor.enableState(BLINKING);
        enableState(FOCUSED);
    }

    void TextInput::draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        typer.draw(target, states);
        if(!cursor.checkState(HIDDEN))
            cursor.draw(target, states);

    }

    void TextInput::setPosition(const sf::Vector2f &position)
    {
    }

    sf::Vector2f TextInput::getPosition() const
    {
    }

    sf::FloatRect TextInput::getGlobalBounds() const
    {
    }

    void TextInput::enableState(Stateful::State state)
    {
        Stateful::enableState(state);
        typer.enableState(state);
        cursor.enableState(state);
    }
}
