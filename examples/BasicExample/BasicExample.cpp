//
// Created by Dave Smith on 3/19/25.
//

#include "BasicExample.h"

#include "../../src/Utilities/TextManipulators.h"

BasicExample::BasicExample() : Application(sf::VideoMode({720, 420}, 32), "Text Input", *this)
{
}

void BasicExample::initialization()
{
    box1.setSize({300, 200});
    box2.setSize({300, 200});
    box2.setPosition({310, 0});
    box3.setSize({400, 50});
    box3.setPosition({0, 350});
    box3.setString("This is a user created event");
    box3.setFillColor(sf::Color::Blue);
    box1.setFillColor(sf::Color::Red);
    box2.setFillColor(sf::Color::Green);
    typer.enableState(ml::Stateful::FOCUSED);
    typer.enableState(ml::Stateful::HIDDEN);
    addComponent(box1);
    addComponent(box2);
    addComponent(box3);
    addComponent(typer);
}

void BasicExample::registerEvents()
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

    /// Preset events
    box1.onFocus(f(box1, "Box 1 is focused try typing"));
    box1.onBlur(f(box1, "Box 1 is blurred you cannot type"));
    box2.onFocus(f(box2, "Box 2 is focused try typing"));
    box2.onBlur(f(box2, "Box 2 is blurred you cannot type"));
    typer.onKeypress([this]()
    {
        std::string str = typer.getString();
        if(box1.checkState(ml::Stateful::FOCUSED))
            box1.setString(str);
        else if(box2.checkState(ml::Stateful::FOCUSED))
            box2.setString(str);
    });

    //// The system automatically manages a component's hover state
    /// so all you need to do is specify what to do on a hover.
    box2.onHover([this]()
    {
        box2.setFillColor(sf::Color::Yellow);
    });
    box2.onUnhover([this](){
        box2.setFillColor(sf::Color::Green);
    });



    //// User created event
    box1.subscribe("myEvent", [this]()
    {
        box1.setString(ml::TextManipulators::wordwrap("MyEvent was Fired", box1.getFont(), box1.getCharacterSize(), box1.getSize().x * .75f));
    });

    box3.onClick([this]()
    {
        box3.publish("myEvent", [this](ml::UIComponent& ) -> bool{
            return !box1.checkState(ml::Stateful::HIDDEN);
        });
    });
}

void BasicExample::update()
{

}
