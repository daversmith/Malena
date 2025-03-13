//
// Created by Dave R. Smith on 3/10/25.
//

#include "JeopardyView.h"

JeopardyView::JeopardyView(JeopardyModel &model)
: Application(sf::VideoMode({720, 420}), "Jeopardy", *this),
model(model), board(model.get_grid_dimensions())
{
}

void JeopardyView::initialize()
{
}
