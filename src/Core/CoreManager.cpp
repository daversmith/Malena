//
// ComponentsManager.cpp
//


#ifndef COMPONENTSMANAGER_TPP
#define COMPONENTSMANAGER_TPP

#include <Malena/Core/CoreManager.h>
#include <algorithm>
namespace ml
{
	template<typename T>
	void CoreManager<T>::addComponent(T &component)
	{
		_components.push_back(&component);
	}
	template<typename T>
	const std::vector<T *> &CoreManager<T>::getComponents()
	{
		return _components;
	}
	template<typename T>
	bool CoreManager<T>::removeComponent(T &component)
	{
		return removeComponent(&component);
	}
	template<typename T>
	bool CoreManager<T>::removeComponent(T *component)
	{
		DeferredOperationsManager<CoreManager<T>>::deferOrExecute([component]() {  // ✅ Using base class method
			CoreManager<T>::doRemoveComponent(component);
		});
		return true;
	}
	template<typename T>
	void CoreManager<T>::doRemoveComponent(T *component)
	{
		auto it = std::find(_components.begin(), _components.end(), component);
		if (it != _components.end())
		{
			_components.erase(it);
		}
	}
	template<typename T>
	void CoreManager<T>::clearComponents()
	{
		deferOrExecute([]() {  // ✅ Using base class method
			CoreManager<T>::_components.clear();
			DeferredOperationsManager<CoreManager<T>>::clearPending();
		});
	}
	template<typename T>
	CoreManager<T>::~CoreManager()
	{
		// Force immediate - we're destructing
		_components.clear();
		DeferredOperationsManager<CoreManager<T>>::clearPending();
	}

} // namespace ml
#endif