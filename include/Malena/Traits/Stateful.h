//
// Created by Dave Smith on 10/5/25.
//

#ifndef STATEFUL_H
#define STATEFUL_H
#include "Flaggable.h"
#include "Malena/Utilities/State.h"

namespace ml
{
    using Stateful = Flaggable<State>;
}

#endif //STATEFUL_H
