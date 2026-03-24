#include <Malena/Engine/Events/Fireable.h>
#include <Malena/Engine/Events/EventManager.h>
#ifndef FIREABLE_TPP
#define FIREABLE_TPP
namespace ml
{
	template<typename EnumType>
	void Fireable::addCallback(EnumType eventEnum,
						 EventReceiver* component,
						 EventCallback callback)
	{
		static_assert(std::is_enum_v<EnumType>,
			"[Malena] Fireable::addCallback — first argument must be an enum value.");

		std::string key = EnumKey::get(eventEnum);
		component->getCallbacks(key).push_back(std::move(callback));
		EventManager::subscribe(eventEnum, component);  // enum passed directly
	}

}
#endif