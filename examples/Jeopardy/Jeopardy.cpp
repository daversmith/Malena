//
// Created by Dave R. Smith on 3/10/25.
//

#include "Jeopardy.h"

Jeopardy::Jeopardy() : view(model), controller(model, view)
{
  view.addController(controller);
}

void Jeopardy::run()
{
  view.run();
}
