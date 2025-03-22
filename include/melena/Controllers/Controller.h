//
// Created by Dave Smith on 3/8/25.
//

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../Interfaces/Updateable.h"
#include "../Managers/ComponentsManager.h"

namespace ml {


    class Controller :  public ComponentsManager
    {
    public:
        virtual void initialization() = 0;
        virtual void registerEvents() = 0;
        virtual ~Controller() = default;

    };

} // namespace ml

#endif //CONTROLLER_H


