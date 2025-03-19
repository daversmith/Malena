//
// Created by Dave R. Smith on 3/10/25.
//

#ifndef JEAPARDYBOARDCONTROLLER_H
#define JEAPARDYBOARDCONTROLLER_H
#include "JeopardyModel.h"
#include "JeopardyView.h"
#include "../../src/Controllers/Controller.h"


class JeopardyBoardController : public ml::Controller
{
  JeopardyModel &model;
  JeopardyView &view;
public:
  JeopardyBoardController(JeopardyModel &model, JeopardyView &view);

  void update() override;

  void initialization() override;

  void registerEvents() override;
};



#endif //JEAPARDYBOARDCONTROLLER_H
