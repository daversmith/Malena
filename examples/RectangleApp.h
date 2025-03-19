//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef RECTANGLEAPP_H
#define RECTANGLEAPP_H
#include "ButtonController.h"
#include "../src/Application/Application.h"
#include "../src/Graphics/Rectangle.h"

class RectangleApp : public ml::Application
{
public:
    RectangleApp();
    void initialization() override;

    void registerEvents() override;

    void update() override;
private:
    ButtonController button_controller_;
};



#endif //RECTANGLEAPP_H
