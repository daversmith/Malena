//
// ComponentsManager.cpp
//


#ifndef COMPONENTSMANAGER_TPP
#define COMPONENTSMANAGER_TPP

#include <Malena/Managers/ComponentsManager.h>
#include <algorithm>
namespace ml
{
	template<typename T>
	void ComponentsManager<T>::addComponent(T &component)
	{
		_components.push_back(&component);
	}
	template<typename T>
	const std::vector<T *> &ComponentsManager<T>::getComponents()
	{
		return _components;
	}
	template<typename T>
	bool ComponentsManager<T>::removeComponent(T &component)
	{
		return removeComponent(&component);
	}
	template<typename T>
	bool ComponentsManager<T>::removeComponent(T *component)
	{
		DeferredOperationsManager<ComponentsManager<T>>::deferOrExecute([component]() {  // ✅ Using base class method
			ComponentsManager<T>::doRemoveComponent(component);
		});
		return true;
	}
	template<typename T>
	void ComponentsManager<T>::doRemoveComponent(T *component)
	{
		auto it = std::find(_components.begin(), _components.end(), component);
		if (it != _components.end())
		{
			_components.erase(it);
		}
	}
	template<typename T>
	void ComponentsManager<T>::clearComponents()
	{
		deferOrExecute([]() {  // ✅ Using base class method
			ComponentsManager<T>::_components.clear();
			DeferredOperationsManager<ComponentsManager<T>>::clearPending();
		});
	}
	template<typename T>
	ComponentsManager<T>::~ComponentsManager()
	{
		// Force immediate - we're destructing
		_components.clear();
		DeferredOperationsManager<ComponentsManager<T>>::clearPending();
	}

} // namespace ml
#endif