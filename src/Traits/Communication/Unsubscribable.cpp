#include <Malena/Traits/Communication/Unsubscribable.h>
#include <Malena/Core/Core.h>
#include <Malena/Engine/Events/EventManager.h>

namespace ml
{
	void Unsubscribable::doUnsubscribe(const std::string& key)
	{
		EventManager::unsubscribe(key, _selfCore ? _selfCore : dynamic_cast<Core*>(this));
	}

	void Unsubscribable::unsubscribeAll()
	{
		EventManager::unsubscribeAll(_selfCore ? _selfCore : dynamic_cast<Core*>(this));
	}
}