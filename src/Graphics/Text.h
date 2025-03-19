//
// Created by Dave Smith on 3/12/25.
//

#ifndef TEXT_H
#define TEXT_H

#include "Shape.h"

namespace ml {

class Text : public virtual Shape<sf::Text>
{
public:
    void set_word_wrap(bool word_wrap);

    void set_max_width(float max_width);

private:
    bool wordWrap = false;
    float maxWidth = 0.f;
public:
    using Shape::Shape;
    void setString(const sf::String &text);
};



} // namespace ml
#endif //TEXT_H


