//
// Created by Dave Smith on 3/10/26.
//

#ifndef FIREABLEBASE_H
#define FIREABLEBASE_H
#include <Malena/Manifests/Manifest.h>
#include <Malena/Traits/Customizable.h>
#include <Malena/Traits/Flaggable.h>

namespace ml
{
    class FireableBase :
    public ml::Customizable<Flaggable, FireableBaseManifest>
    {};
}

#endif //FIREABLEBASE_H
