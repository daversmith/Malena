//
// ComponentsManager.cpp
//

#include <Malena/Managers/ComponentsManager.h>
#include <algorithm>

namespace ml
{
	void ComponentsManager::addComponent(UIComponent &component)
	{
		uiComponents.push_back(&component);
	}

	const std::vector<UIComponent *> &ComponentsManager::getUIComponents()
	{
		return uiComponents;
	}

	bool ComponentsManager::removeComponent(UIComponent &component)
	{
		return removeComponent(&component);
	}

	bool ComponentsManager::removeComponent(UIComponent *component)
	{
		deferOrExecute([component]() {  // ✅ Using base class method
			ComponentsManager::doRemoveComponent(component);
		});
		return true;
	}

	void ComponentsManager::doRemoveComponent(UIComponent *component)
	{
		auto it = std::find(uiComponents.begin(), uiComponents.end(), component);
		if (it != uiComponents.end())
		{
			uiComponents.erase(it);
		}
	}

	void ComponentsManager::clearComponents()
	{
		deferOrExecute([]() {  // ✅ Using base class method
			ComponentsManager::uiComponents.clear();
			clearPending();
		});
	}

	ComponentsManager::~ComponentsManager()
	{
		// Force immediate - we're destructing
		uiComponents.clear();
		clearPending();
	}

} // namespace ml