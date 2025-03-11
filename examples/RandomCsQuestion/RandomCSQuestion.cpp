//
// Created by Dave R. Smith on 3/11/25.
//

#include "RandomCSQuestion.h"

RandomCSQuestion::RandomCSQuestion()
  : view(model), controller(model, view)
{
  view.addController(controller);
}

void RandomCSQuestion::run()
{
  view.run();
}
