//
// Created by Dave Smith on 3/19/25.
//

#include "TextInputTest.h"

#include "../../src/Utilities/TextManipulators.h"

TextInputTest::TextInputTest() : Application(sf::VideoMode({720, 420}, 32), "Text Input", *this)
{
}

void TextInputTest::initialization()
{
    box1.setSize({300, 200});
    box2.setSize({300, 200});
    box2.setPosition({310, 0});
    box1.setFillColor(sf::Color::Red);
    box2.setFillColor(sf::Color::Green);
    typer.enableState(ml::Stateful::FOCUSED);
    typer.enableState(ml::Stateful::HIDDEN);
    addComponent(box1);
    addComponent(box2);
    addComponent(typer);
}

void TextInputTest::registerEvents()
{
    auto f = [this](ml::RectangleButton& box, const std::string& text)
    {
        return [&box, this, text]()
        {
            box.setString(ml::TextManipulators::wordwrap(text, box.getFont(), box.getCharacterSize(), box.getSize().x * .75f));
            box.setPosition(box.getPosition());
            typer.setString("");
        };
    };

    box1.onClick([](){});
    box2.onClick([](){});
    box1.onFocus(f(box1, "Box 1 is focused try typing"));
    box1.onBlur(f(box1, "Box 1 is blurred you cannot type"));
    box2.onFocus(f(box2, "Box 2 is focused try typing"));
    box2.onBlur(f(box2, "Box 2 is blurred blurred you cannot type"));

    typer.onKeypress([this]()
    {
        std::string str = typer.getString();
        if(box1.checkState(ml::Stateful::FOCUSED))
            box1.setString(str);
        else if(box2.checkState(ml::Stateful::FOCUSED))
            box2.setString(str);
    });
}
