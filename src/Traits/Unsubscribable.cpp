#include <Malena/Traits/Unsubscribable.h>
#include <Malena/Core/Core.h>
#include <Malena/Engine/Events/_EventsManager.h>

namespace ml
{
	void Unsubscribable::doUnsubscribe(const std::string& key)
	{
		_EventsManager::unsubscribe(key, dynamic_cast<Core*>(this));
	}

	void Unsubscribable::unsubscribeAll()
	{
		_EventsManager::unsubscribeAll(dynamic_cast<Core*>(this));
	}
}