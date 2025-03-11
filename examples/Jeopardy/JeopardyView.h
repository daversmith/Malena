//
// Created by Dave R. Smith on 3/10/25.
//

#ifndef JEOPARDYVIEW_H
#define JEOPARDYVIEW_H
#include "JeopardyModel.h"
#include "../../src/Application.h"
#include "../../src/Graphics/Grid.h"
#include "../../src/Graphics/RectangleButton.h"


class JeopardyView : public Application
{
  JeopardyModel &model;
public:
  Grid board;
  RectangleButton questionBoard;

  JeopardyView(JeopardyModel &model);
  void initialize();
};



#endif //JEOPARDYVIEW_H
