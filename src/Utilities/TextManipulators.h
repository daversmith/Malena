//
// Created by Dave R. Smith on 3/12/25.
//

#ifndef TEXTMANIPULATORS_H
#define TEXTMANIPULATORS_H

#include <string>

#include "SFML/Graphics/Text.hpp"
#include <iostream>

namespace ml {
class TextManipulators {

public:
  static  std::string wordwrap(sf::Text& text, float maxwidth);
};


} // namespace ml
#endif //TEXTMANIPULATORS_H


