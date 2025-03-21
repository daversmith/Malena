//
// Created by Dave Smith on 3/19/25.
//

#ifndef TEXTINPUTTEST_H
#define TEXTINPUTTEST_H
#include "../../src/Application/Application.h"
#include "../../src/Graphics/Cursor.h"
#include "../../src/Graphics/RectangleButton.h"
#include "../../src/Graphics/TextInput.h"
#include "../../src/Graphics/Typer.h"


class BasicExample : public ml::Application
{
    ml::RectangleButton box1, box2, box3;
    ml::Typer typer;

public:
    BasicExample();
    void initialization() override;
    void registerEvents() override;
    void update() override;
};



#endif //TEXTINPUTTEST_H
