
#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "Controller.h"
#include "../Adapters/UIComponentAdapter.h"
namespace ml {

    class UIController :  public Controller
    {
        inline static std::vector<Controller *> controllers;
        UIComponentAdapter proxy;
    public:
        using Controller::Controller;
        UIController(){ UIController::registerEvents();}
        const std::vector<Controller *> &getControllers() const;
        void addController(Controller& controller);

        virtual void update() ;

        virtual void initialization() ;

        virtual void registerEvents() ;

        void onUpdate(std::function<void()> f);
        virtual ~UIController() = default;
    };

} // namespace ml


#endif //GAMELOGIC_H

