//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef GAMELOGIC_H
#define GAMELOGIC_H
#include "ComponentsManager.h"


class AppLogic : public ComponentsManager
{

public:
    virtual void initialization() = 0;
    virtual void registerEvents() = 0;
    virtual void update() = 0;
};



#endif //GAMELOGIC_H
