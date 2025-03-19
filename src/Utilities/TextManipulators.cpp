//
// Created by Dave R. Smith on 3/12/25.
//

#include "TextManipulators.h"

namespace ml {

 std::string TextManipulators::wordwrap(sf::Text &text, float maxwidth)
{
   std::string temp;
   std::string s = text.getString();
   int start = 0, i = 0;
   do {
     int space_pos = s.find(' ', start);
     std::string sub = s.substr(start, space_pos - start);
     std::string temp2 = temp + (temp.empty() ? "" : " ") + sub;
     text.setString(temp2);

     if (text.getGlobalBounds().size.x > maxwidth) {
       temp += '\n'; // Insert a new line before adding more text
       temp2 = temp + sub;
       text.setString(temp2); // Update text to include new line
     } else {
       temp = temp2; // Update temp to the latest valid string
     }

     if (space_pos == std::string::npos) {
       break; // No more spaces, exit the loop
     }
     start = space_pos + 1; // Move start position past the last space
   } while (true);

   return text.getString();
}
} // namespace ml

