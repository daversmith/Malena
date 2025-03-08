//
// Created by Dave Smith on 3/7/25.
//

#ifndef APPLICATION_H
#define APPLICATION_H
#include "AppController.h"
#include "UIComponentAdapter.h"
#include "UIManager.h"

class Application : public UIManager, public AppController
{
public:
    using UIManager::UIManager;
};

#endif //APPLICATION_H
