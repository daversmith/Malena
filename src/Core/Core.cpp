//
// Created by Dave Smith on 10/4/25.
//

#include <Malena/Core/Core.h>
namespace ml
{

    Core::~Core()
    {
        EventManager::unsubscribeAll(this);
    }

} // namespace ml
