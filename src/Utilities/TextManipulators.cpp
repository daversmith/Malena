//
// Created by Dave R. Smith on 3/12/25.
//

#include "TextManipulators.h"

namespace ml {

 std::string TextManipulators::wordwrap(sf::Text &text, float maxwidth)
{
   return wordwrap(text.getString(), text.getFont(), text.getCharacterSize(), maxwidth);
}
  std::string TextManipulators::wordwrap(const std::string& input, const sf::Font& font, unsigned int characterSize, float maxWidth)
 {
   std::istringstream iss(input);
   std::string word;
   std::string line;
   std::string result;

   sf::Text text(font);
   text.setCharacterSize(characterSize);


   while (iss >> word)
   {
     std::string testLine = line.empty() ? word : line + " " + word;
     text.setString(testLine);

     if (text.getGlobalBounds().size.x > maxWidth)
     {
       if (!line.empty())
       {
         result += line + "\n";
         line = word;
       }
       else
       {
         result += word + "\n";
         line = "";
       }
     }
     else
     {
       line = testLine;
     }
   }


   if (!line.empty())
   {
     result += line;
   }

   return result;
 }

} // namespace ml

