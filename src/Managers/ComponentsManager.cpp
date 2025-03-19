//
// Created by Dave R. Smith on 3/6/25.
//

#include "ComponentsManager.h"

namespace ml {
void ComponentsManager::addComponent(UIComponent &component)
{
  uiComponents.push_back(&component);
}

const std::vector<UIComponent *> &ComponentsManager::getUIComponents() const
{
  return uiComponents;
}



ComponentsManager::~ComponentsManager()
{
  for(auto &c : uiComponents)
    if(c->isDynamic)
      delete c;
}
} // namespace ml

