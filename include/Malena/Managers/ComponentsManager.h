//
// ComponentsManager.h
//

#ifndef COMPONENTSMANAGER_H
#define COMPONENTSMANAGER_H

#pragma once

#include <vector>
#include <Malena/Interfaces/UIComponent.h>
#include <Malena/Managers/DeferredOperationsManager.h>

namespace ml
{
	class ComponentsManager : public DeferredOperationsManager<ComponentsManager>
	{
		std::vector<UIComponent *> uiComponents;

	public:
		[[nodiscard]] const std::vector<UIComponent *> &getUIComponents() const;

		virtual void addComponent(UIComponent &component);

		// Safe to call anytime - defers removal if currently iterating
		virtual bool removeComponent(UIComponent &component);
		virtual bool removeComponent(UIComponent *component);

		// Clear all (deferred if busy)
		virtual void clearComponents();

		virtual ~ComponentsManager();

	private:
		// Internal method for actual removal
		void doRemoveComponent(UIComponent *component);
	};

} // namespace ml

#endif // COMPONENTSMANAGER_H