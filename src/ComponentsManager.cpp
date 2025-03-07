//
// Created by Dave R. Smith on 3/6/25.
//

#include "ComponentsManager.h"
void ComponentsManager::addComponent(Component &component)
{
  components.push_back(&component);
}

std::vector<Component *> ComponentsManager::getComponents() const
{
  return components;
}
