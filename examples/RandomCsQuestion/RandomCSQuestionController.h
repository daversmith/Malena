//
// Created by Dave R. Smith on 3/11/25.
//

#ifndef RANDOMCSQUESTIONCONTROLLER_H
#define RANDOMCSQUESTIONCONTROLLER_H
#include "RandomCSQuestionModel.h"
#include "RandomCSQuestionView.h"
#include "../../src/Controller.h"


class RandomCSQuestionController : public Controller
{
  RandomCSQuestionModel &model;
  RandomCSQuestionView &view;
public:
  RandomCSQuestionController(RandomCSQuestionModel &model,
    RandomCSQuestionView &view);
  void update() override;

  void initialization() override;

  void registerEvents() override;
};



#endif //RANDOMCSQUESTIONCONTROLLER_H
