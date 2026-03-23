//
// Created by Dave Smith on 3/20/26.
//

#ifndef REACTIVE_H
#define REACTIVE_H
#include "Fireable.h"
#include "Malena/Traits/Subscribable.h"


namespace ml
{
	class Reactive : public virtual Fireable//, public Subscribable
	{
		using Fireable::Fireable;
	};
}
#endif //REACTIVE_H
