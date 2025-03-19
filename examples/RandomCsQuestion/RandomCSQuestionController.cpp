//
// Created by Dave R. Smith on 3/11/25.
//

#include "RandomCSQuestionController.h"

RandomCSQuestionController::RandomCSQuestionController(RandomCSQuestionModel &model, RandomCSQuestionView &view)
  : model(model), view(view)
{
}

void RandomCSQuestionController::update()
{

}

void RandomCSQuestionController::initialization()
{
  addComponent(view.next);
}

void RandomCSQuestionController::registerEvents()
{
  view.next.onClick([this]()
  {
    view.next.enableState(ml::Stateful::CLICKED);
  });
}
