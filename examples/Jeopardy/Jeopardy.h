//
// Created by Dave R. Smith on 3/10/25.
//

#ifndef JEOPARDY_H
#define JEOPARDY_H
#include "JeopardyBoardController.h"
#include "JeopardyModel.h"
#include "../../src/Application/Application.h"
#include "../../src/Graphics/Grid.h"
#include "../../src/Graphics/RectangleButton.h"


class Jeopardy
{

  JeopardyModel model;
  JeopardyView view;
  JeopardyBoardController controller;

public:
  Jeopardy();
  void run();
};



#endif //JEOPARDY_H
