//
// Created by Dave Smith on 10/4/25.
//

#include <Malena/Core/Core.h>
#include <Malena/Engine/App/Application.h>

namespace ml
{

    Core::~Core()
    {
        _EventsManager::unsubscribeAll(this);
    }

} // namespace ml
