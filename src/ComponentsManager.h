//
// Created by Dave R. Smith on 3/6/25.
//

#ifndef COMPONENTSMANAGER_H
#define COMPONENTSMANAGER_H

#include <vector>
#include "Component.h"

class ComponentsManager {
  std::vector<Component *> components;

public:
  [[nodiscard]] std::vector<Component *> getComponents() const;

  void addComponent(Component & component);

};



#endif //COMPONENTSMANAGER_H
