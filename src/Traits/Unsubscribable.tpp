#include <Malena/Traits/Unsubscribable.h>
#include <Malena/Engine/Events/_EventsManager.h>
#include <Malena/Core/Core.h>
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