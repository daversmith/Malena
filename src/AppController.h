//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "Controller.h"


class AppController :  public Controller
{
    inline static std::vector<Controller *> controllers;
public:
    using Controller::Controller;
    const std::vector<Controller *> &getControllers() const;
    void addController(Controller& controller);

    virtual void update() ;

    virtual void initialization() ;

    virtual void registerEvents() ;

    virtual ~AppController() = default;
};



#endif //GAMELOGIC_H
