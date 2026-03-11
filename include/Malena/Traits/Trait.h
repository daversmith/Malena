//
// Created by Dave Smith on 3/11/26.
//

#ifndef TRAIT_H
#define TRAIT_H
#include "Customizable.h"

class Trait {};

template<typename Manifest>
class TraitWith : public Trait,
                  public ml::Customizable<Manifest>
{};
#endif //TRAIT_H
