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
	const std::vector<T *> &CoreManager<T>::getComponents() const
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
		this->deferOrExecute([this, component]() {
			doRemoveComponent(component);
		});
		return true;
	}
	template<typename T>
	void CoreManager<T>::doRemoveComponent(T *component)
	{
		auto it = std::find(_components.begin(), _components.end(), component);
		if (it != _components.end())
			_components.erase(it);
	}
	template<typename T>
	void CoreManager<T>::clear()
	{
		this->deferOrExecute([this]() {
			_components.clear();
		});
	}
	template<typename T>
	CoreManager<T>::~CoreManager()
	{
		_components.clear();
	}

} // namespace ml
#endif