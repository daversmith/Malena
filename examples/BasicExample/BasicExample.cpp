//
// Created by Dave Smith on 3/19/25.
//

#include "BasicExample.h"

#include <melena/Utilities/TextManipulators.h>

BasicExample::BasicExample() : Application(sf::VideoMode({720, 420}, 32), "Text Input", *this)
{
}

void BasicExample::initialization()
{
    box1.setSize({300, 200});
    box1.setFillColor(sf::Color::Red);
    box2.setFillColor(sf::Color::Green);

    box2.setSize({300, 200});
    box2.setPosition({310, 0});

    myEventButton.setSize({500, 50});
    myEventButton.setPosition({0, 350});
    myEventButton.setString("This triggers a user created event");
    myEventButton.setFillColor(sf::Color::Blue);

    unsubscribeBtn.setSize({300, 50});
    unsubscribeBtn.setFillColor(sf::Color::Cyan);
    unsubscribeBtn.setString("Unsubscribe MyEvent");

    subscribeBtn.setSize(unsubscribeBtn.getSize());
    subscribeBtn.setFillColor(sf::Color::Red);
    subscribeBtn.setString("Subscribe MyEvent");

    //position unsubscribeBtn below box1
    box1.bottom(unsubscribeBtn, 20);
    //position subscribeBtn below box2
    box2.bottom(subscribeBtn, 20);

    unsubscribeAll.setSize(subscribeBtn.getSize());
    unsubscribeAll.setFillColor(sf::Color(125u, 125u, 125u));
    unsubscribeAll.setString("Unsubscribe All");
    //position unsubscribeAll below unsubscribeBtn;
    unsubscribeBtn.bottom(unsubscribeAll, 20);

    clearAll.setSize(subscribeBtn.getSize());
    clearAll.setFillColor(sf::Color(125u, 0, 125u));
    clearAll.setString("Clear All");
    //position unsubscribeAll below unsubscribeBtn;
    unsubscribeAll.right(clearAll, 10);

    typer.enableState(ml::Stateful::ENABLED);
    typer.enableState(ml::Stateful::HIDDEN);

    addComponent(box1);
    addComponent(box2);
    addComponent(myEventButton);
    addComponent(unsubscribeBtn);
    addComponent(subscribeBtn);
    addComponent(unsubscribeAll);
    addComponent(clearAll);
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
    /// In this example, the text in the boxes will show whether or not the boxes ar focused or blurred (unfocused)
    box1.onFocus(f(box1, "Box 1 is focused try typing"));
    box1.onBlur(f(box1, "Box 1 is blurred you cannot type"));
    box2.onFocus(f(box2, "Box 2 is focused try typing"));
    box2.onBlur(f(box2, "Box 2 is blurred you cannot type"));

    /// This example is showing that you can register an event when a key is pressed
    /// Here, when a key is pressed, which ever box is focused whill get the text typed in Typer
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
    /// the example here is say to change the color of box2 to yellow when hovered
    box2.onHover([this]()
    {
        box2.setFillColor(sf::Color::Yellow);
    });

    //The example here is saying to change the color of box2 back to blue when box2 is no longer hovered
    box2.onUnhover([this](){
        box2.setFillColor(sf::Color::Green);
    });

    /// use update to check states and update component accourding to their state
    /// HThe example here is saying when box1 is in a hovered state, change the color of myEventButton
    box2.onUpdate([this]()
    {
        if(box1.checkState(ml::Stateful::HOVERED))
            myEventButton.setFillColor(sf::Color::Magenta);
        else
            myEventButton.setFillColor(sf::Color::Blue);
    });

    auto myEvent = [this]()
    {
        box1.setString(ml::TextManipulators::wordwrap("MyEvent was Fired", box1.getFont(), box1.getCharacterSize(), box1.getSize().x * .75f));

        //this removes a specific event from a component
        box2.unsubscribe("update");
    };
    //// User created event
    /// Here you can register to your own event
    box1.subscribe("myEvent", myEvent );

    unsubscribeBtn.onClick([this]()
    {
        // This removes all events registered by a component
        box1.unsubscribe("myEvent");
    });

    subscribeBtn.onClick([this, myEvent]()
    {
        box1.subscribe("myEvent", myEvent );
    });
    unsubscribeAll.onClick([this]()
    {
        box1.unsubscribeAll();
    });

    ///you can also remove all events
    clearAll.onClick([this]()
    {
        ml::EventsManager::clearAllEvents();
    });
    /// when you register your own event, it's up to you to publish the event
    /// In this example, we are publishing "myEvent" when myEventButton is clicked
    myEventButton.onClick([this]()
    {
        myEventButton.publish("myEvent", [this](ml::UIComponent& ) -> bool{
            return !box1.checkState(ml::Stateful::HIDDEN);
        });
    });

}

