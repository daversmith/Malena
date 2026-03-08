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
		inline static std::vector<UIComponent *> uiComponents;

	public:
		[[nodiscard]] static const std::vector<UIComponent *> &getUIComponents();

		static void addComponent(UIComponent &component);

		// Safe to call anytime - defers removal if currently iterating
		static bool removeComponent(UIComponent &component);
		static bool removeComponent(UIComponent *component);

		// Clear all (deferred if busy)
		static void clearComponents();

		virtual ~ComponentsManager();

	private:
		// Internal method for actual removal
		static void doRemoveComponent(UIComponent *component);
	};

} // namespace ml

#endif // COMPONENTSMANAGER_H