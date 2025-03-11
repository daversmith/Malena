//
// Created by Dave Smith on 3/8/25.
//

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Updateable.h"
#include "ComponentsManager.h"

class Controller : public Updateable, public ComponentsManager
{
public:
    virtual void initialization() = 0;
    virtual void registerEvents() = 0;
    virtual ~Controller() = default;

};



#endif //CONTROLLER_H
