#include <Malena/Core/Core.h>
#include <Malena/Engine/Events/EventManager.h>
#include <Malena/Traits/Unsubscribable.h>

namespace ml
{
	void Unsubscribable::doUnsubscribe(const std::string& key)
	{
		EventsManager::unsubscribe(key, dynamic_cast<Core*>(this));
	}

	void Unsubscribable::unsubscribeAll()
	{
		EventsManager::unsubscribeAll(dynamic_cast<Core*>(this));
	}
}