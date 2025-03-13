//
// Created by Dave R. Smith on 3/11/25.
//

#include "RandomCSQuestionView.h"

RandomCSQuestionView::RandomCSQuestionView(RandomCSQuestionModel &model)
  : Application(sf::VideoMode({720u, 420}, 32u), "Random CS Generator", *this),
    model(model), question(model.getFont()),
  next(model.getFont(), (sf::Vector2f)model.getButtonSize())
{
}

void RandomCSQuestionView::initialization()
{
  // Application::initialization();
  model.loadQuestions("examples/Jeopardy/questions/jeopardy_cpp_questions.txt");
  model.setButtonName("Next");
  next.setString(model.getButtonName());
  addComponent(next);
}

void RandomCSQuestionView::registerEvents()
{
  Application::registerEvents();
}

void RandomCSQuestionView::update()
{
  // Application::update();
 if(next.checkState(StateManager::CLICKED))
 {
   question.setString(model.getNextQuestion());
   next.disableState(StateManager::CLICKED);
 }
}
