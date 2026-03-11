//
// Created by Dave Smith on 10/5/25.
//

#ifndef STATEFUL_H
#define STATEFUL_H
#include "../Managers/FlagManager.h"
#include "Malena/Utilities/Flag.h"

namespace ml
{
    class Flaggable : public FlagManager<Flag>{};
}

#endif //STATEFUL_H
