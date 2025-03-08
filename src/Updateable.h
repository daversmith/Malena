//
// Created by Dave Smith on 3/8/25.
//

#ifndef UPDATEABLE_H
#define UPDATEABLE_H
class Updateable
{
public:
    virtual ~Updateable() = default;

    virtual void update() = 0;
};
#endif //UPDATEABLE_H
