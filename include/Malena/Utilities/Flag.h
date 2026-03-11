//
// Created by Dave Smith on 10/5/25.
//

#ifndef SYSTEMSTATESENUM_H
#define SYSTEMSTATESENUM_H
namespace ml
{
    enum class Flag
    {
        HOVERED,
        CLICKED,
        HIDDEN,
        BLINKING,
        FOCUSED,
        ENABLED,
        DRAGGABLE,
        DRAGGING,
        VERTICAL,
        HORIZONTAL,
        //// do not add states below
        LAST_FLAG
    };
}

#endif //SYSTEMSTATESENUM_H
