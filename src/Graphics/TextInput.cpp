
#include <melena/Graphics/TextInput.h>

#include <melena/Traits/Stateful.h>

namespace ml
{
    TextInput::TextInput()
    {
        cursor.enableState(Stateful::BLINKING);
        enableState(Stateful::FOCUSED);
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
        return cursor.getPosition();
    }

    sf::FloatRect TextInput::getGlobalBounds() const
    {
        return cursor.getGlobalBounds();
    }

    void TextInput::enableState(const Stateful::State state)
    {
        Stateful::enableState(state);
        typer.enableState(state);
        cursor.enableState(state);
    }
}
