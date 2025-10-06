//
// Created by Dave Smith on 10/5/25.
//

#ifndef CONTEXT_H
#define CONTEXT_H
#include <Malena/Managers/AssetsManager.h>
#include <Malena/Managers/ConfigManager.h>
#include <Malena/Managers/StateManager.h>
namespace ml
{
    template<typename Manifest>
    struct Context {
        using Assets = AssetsManager<Manifest>;
        using Config = ConfigManager<Manifest>;
        using State = StateManager<Manifest>;
    };
}

#endif //CONTEXT_H
