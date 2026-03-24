#include <Malena/Core/Core.h>
#include <Malena/Engine/Events/EventsManager.h>
#include <Malena/Traits/Unsubscribable.h>
namespace ml
{
	// template<typename EnumType>
	// void Unsubscribable::unsubscribe(EnumType event)
	// {
	// 	static_assert(std::is_enum_v<EnumType>,
	// 		"[Malena] unsubscribe — argument must be an enum value.");
	//
	// 	_EventsManager::unsubscribe(EnumKey::get(event), dynamic_cast<Core*>(this));
	// }
}