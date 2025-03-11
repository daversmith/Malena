//
// Created by Dave R. Smith on 3/10/25.
//

#include "JeopardyBoardController.h"

JeopardyBoardController::JeopardyBoardController(JeopardyModel &model, JeopardyView &view)
  : model(model), view(view)
{
}

void JeopardyBoardController::update()
{

}

void JeopardyBoardController::initialization()
{
  auto gs = model.get_grid_size();
  auto gd = model.get_grid_dimensions();
  auto spacing = model.get_spacing();
  view.questionBoard = RectangleButton(FontManager::DEFAULT, model.get_question_window_size());
  model.set_filename("examples/Jeopardy/questions/jeopardy_cpp_questions.txt");
  sf::Vector2f boxSize = {gs.x/(gd.x + spacing),
                            gs.y/(gd.y + spacing)};
  for(int i=0; i<gd.x * gd.y; i++)
    view.board.addComponent(*(new RectangleButton(FontManager::DEFAULT,boxSize, "100")));
  view.board.addToApp(*this);
  addComponent(view.questionBoard);
}

void JeopardyBoardController::registerEvents()
{
  auto f = [this]()
  {
    if(model.has_more_questions())
    {
      ;
      auto question = model.get_next_question();
      view.questionBoard.setString(question.first);
      view.questionBoard.disableState(StateManager::HIDDEN);
    }
  };
  for(auto& c : view.board.getUIComponents())
    c->onClick(f);
}
