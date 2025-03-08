//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef RECTANGLEAPP_H
#define RECTANGLEAPP_H
#include "../src/Application.h"
#include "../src/UIManager.h"
#include "../src/Graphics/Rectangle.h"

class RectangleApp : public Application
{
    Rectangle square;
    Rectangle square2;

public:
    RectangleApp();
    void initialization() override;

    void registerEvents() override;

    void update() override;
};



#endif //RECTANGLEAPP_H
