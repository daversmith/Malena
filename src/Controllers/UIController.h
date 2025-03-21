
#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "Controller.h"
#include "../Adapters/UIComponentAdapter.h"
namespace ml {

    class UIController :  public Controller
    {
        UIComponentAdapter proxy;
    public:
        using Controller::Controller;

        virtual void initialization() = 0;

        virtual void registerEvents() = 0;

        void onUpdate(std::function<void()> f);
        virtual ~UIController() = default;
    };

} // namespace ml


#endif //GAMELOGIC_H

