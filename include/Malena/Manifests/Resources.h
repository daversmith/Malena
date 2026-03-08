//
// Created by Dave Smith on 3/7/26.
//

#ifndef RESOURCES_H
#define RESOURCES_H

#include <Malena/Manifests/Context.h>
#include <Malena/Manifests/ManifestAliases.h>

namespace ml
{
    template <typename Manifest>
    struct Resources : public Context<Manifest>, public ManifestAliases<Manifest>{};
}


#endif //RESOURCES_H
