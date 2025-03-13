//
// Created by Dave R. Smith on 3/11/25.
//

#ifndef RANDOMCSQUESTIONVIEW_H
#define RANDOMCSQUESTIONVIEW_H
#include "RandomCSQuestionModel.h"
#include "../../src/Application.h"
#include "../../src/Graphics/RectangleButton.h"


class RandomCSQuestionView
  : public Application
{
  RectangleButton next;
  sf::Text question;
  RandomCSQuestionModel &model;

public:
  RandomCSQuestionView(RandomCSQuestionModel& model);
  void initialization() override;
  void registerEvents() override;
  void update() override;
  friend class RandomCSQuestionController;
};



#endif //RANDOMCSQUESTIONVIEW_H
