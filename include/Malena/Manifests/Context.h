//
// Created by Dave Smith on 10/5/25.
//

#ifndef CONTEXT_H
#define CONTEXT_H
#include <Malena/Resources/AssetsManager.h>
#include <Malena/Resources/ConfigManager.h>
#include <Malena/Resources/StateManager.h>
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
