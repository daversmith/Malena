//
// Created by Dave R. Smith on 3/6/25.
//

#ifndef COMPONENTSMANAGER_H
#define COMPONENTSMANAGER_H

#include <vector>
#include "UIComponent.h"

class ComponentsManager {
  std::vector<UIComponent *> uiComponents;
public:
  [[nodiscard]] std::vector<UIComponent *> getUIComponents() const;

  void addComponent(UIComponent & component);
  ~ComponentsManager();

};



#endif //COMPONENTSMANAGER_H
