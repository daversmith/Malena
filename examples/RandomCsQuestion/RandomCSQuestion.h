//
// Created by Dave R. Smith on 3/11/25.
//

#ifndef RANDOMCSQUESTION_H
#define RANDOMCSQUESTION_H
#include "RandomCSQuestionModel.h"
#include "RandomCSQuestionView.h"
#include "RandomCSQuestionController.h"

class RandomCSQuestion
{
  RandomCSQuestionModel model;
  RandomCSQuestionView view;
  RandomCSQuestionController controller;

public:
  RandomCSQuestion();
  void run();
};



#endif //RANDOMCSQUESTION_H
