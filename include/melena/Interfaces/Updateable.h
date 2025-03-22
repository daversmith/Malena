//
// Created by Dave Smith on 3/8/25.
//

#ifndef UPDATEABLE_H
#define UPDATEABLE_H
#include <optional>
#include <SFML/Graphics.hpp>
namespace ml {

class Updateable
{
public:
    virtual ~Updateable() = default;

    virtual void update() = 0;
};
} // namespace ml
#endif //UPDATEABLE_H


