//
// Created by Dave Smith on 10/5/25.
//

#ifndef STATEFUL_H
#define STATEFUL_H
#include <Malena/Managers/FlagManager.h>
#include <Malena/Utilities/Flag.h>
#include <Malena/Traits/Trait.h>
namespace ml
{
    class Flaggable : public Trait, public FlagManager<Flag>{};
}

#endif //STATEFUL_H
