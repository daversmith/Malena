//
// Created by Dave Smith on 3/11/26.
//

#ifndef TRAIT_H
#define TRAIT_H
#include <Malena/Traits/Customizable.h>
namespace ml
{
	class Trait {};

	template<typename Manifest>
	class TraitWith : public Trait,
					  public ml::Customizable<Manifest>
	{};
}
#endif //TRAIT_H
