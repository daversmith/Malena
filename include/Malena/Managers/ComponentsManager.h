//
// Created by Dave R. Smith on 3/6/25.
//

#ifndef COMPONENTSMANAGER_H
#define COMPONENTSMANAGER_H

#pragma once

#include <vector>
#include "../Interfaces/UIComponent.h"

namespace ml
{


	class ComponentsManager
	{
		std::vector<UIComponent *> uiComponents;

	public:
		[[nodiscard]] const std::vector<UIComponent *> &getUIComponents() const;

		virtual void addComponent(UIComponent &component);

		virtual ~ComponentsManager();
	};

} // namespace ml

#endif // COMPONENTSMANAGER_H
