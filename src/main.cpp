#include <SFML/Graphics.hpp>

#include "../examples/RectangleApp.h"
#include "Graphics/Circle.h"
#include "Graphics/Grid.h"

int main()
{
    RectangleApp app;
    Rectangle r;
    r.setSize({50.f, 50.f});

    Rectangle r1({50.f, 50.f}), r2({50.f, 50.f}), r3({50.f, 50.f});
    Circle r4(25.f), r5(25.f), r6(25.f);
    Grid grid({3, 2});

    grid.addComponent(r1);
    grid.addComponent(r2);
    grid.addComponent(r3);
    grid.addComponent(r4);
    grid.addComponent(r5);
    grid.addComponent(r6);


    r.onClick([&r]()
    {
       // r.setSize({300.f, 300.f});
    });

    r.onUpdate([&r]()
    {
        // if(r.checkState(StateManager::CLICKED))
            r.move({0, .05f});
    });

    app.onUpdate([&r]()
    {
        if(r.getPosition().y > 200)
            r.setSize({300.f, 300.f});
    });

    app.addComponent(grid);
    app.addComponent(r);
    app.run();
}
