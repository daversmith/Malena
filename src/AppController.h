//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef GAMELOGIC_H
#define GAMELOGIC_H
#include "ComponentsManager.h"
#include "Controller.h"


class AppController : public ComponentsManager, public Controller
{
    inline static std::vector<Controller *> controllers;
public:

    void addController(Controller& controller);

    void update() override;

    void initialization() override;

    void registerEvents() override;

    virtual ~AppController() = default;
};



#endif //GAMELOGIC_H
