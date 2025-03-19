//
// Created by Dave R. Smith on 3/10/25.
//

#ifndef JEOPARDYVIEW_H
#define JEOPARDYVIEW_H
#include "JeopardyModel.h"
#include "../../src/Application/Application.h"
#include "../../src/Graphics/Grid.h"
#include "../../src/Graphics/RectangleButton.h"


class JeopardyView : public ml::Application
{
  JeopardyModel &model;
public:
  ml::Grid board;
  ml::RectangleButton questionBoard;

  JeopardyView(JeopardyModel &model);
  void initialize();
};



#endif //JEOPARDYVIEW_H
