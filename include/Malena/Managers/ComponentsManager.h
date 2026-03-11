//
// ComponentsManager.h
//

#ifndef COMPONENTSMANAGER_H
#define COMPONENTSMANAGER_H

#pragma once

#include <vector>
#include <Malena/Interfaces/Core.h>
#include <Malena/Managers/DeferredOperationsManager.h>

namespace ml
{
	// template<t?

	template<typename T>
	class ComponentsManager : public DeferredOperationsManager<ComponentsManager<T>>
	{
		inline static std::vector<T *> _components;

	public:
		[[nodiscard]] static const std::vector<T *> &getComponents();

		static void addComponent(T &component);

		// Safe to call anytime - defers removal if currently iterating
		static bool removeComponent(T &component);
		static bool removeComponent(T *component);

		// Clear all (deferred if busy)
		static void clearComponents();

		virtual ~ComponentsManager();

	private:
		// Internal method for actual removal
		static void doRemoveComponent(T *component);
	};

} // namespace ml
#include "../../../src/Managers/ComponentsManager.cpp"
#endif // COMPONENTSMANAGER_H