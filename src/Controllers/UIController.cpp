//
// Created by Dave R. Smith on 3/5/25.
//

#include "UIController.h"

namespace ml {

    void UIController::addController(Controller &controller)
    {
        controllers.push_back(&controller);
    }

    void UIController::update()
    {
        for(auto &c : controllers)
            c->update();
    }

    void UIController::initialization()
    {
        for(auto &c : controllers)
            c->initialization();
        UIController::addComponent(proxy);
    }

    void UIController::registerEvents()
    {
        for(auto &c : controllers)
            c->initialization();
    }

    const std::vector<Controller *> & UIController::getControllers() const
    {
        return controllers;
    }

    void UIController::onUpdate(std::function<void()> f) {
        proxy.onUpdate(f);
    }
} // namespace ml

