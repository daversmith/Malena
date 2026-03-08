//
// Created by Dave Smith on 10/5/25.
//

#ifndef CONTEXT_H
#define CONTEXT_H
#include <Malena/Managers/AssetsManager.h>
#include <Malena/Managers/ConfigManager.h>
#include <Malena/Managers/StateManager.h>
#include <Malena/Manifests/ManifestAliases.h>
namespace ml
{
    template<typename Manifest>
    struct Context : public ManifestAliases<Manifest> {
        using AssetMgr = AssetsManager<Manifest>;
        using ConfigMgr = ConfigManager<Manifest>;
        using StateMgr = StateManager<Manifest>;
    };
}

#endif //CONTEXT_H
