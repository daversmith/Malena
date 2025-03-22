
#include <melena/Graphics/Text.h>

#include <melena/Utilities/TextManipulators.h>

namespace ml {
    Text::Text(const sf::Font &font) : Shape<sf::Text>(font)
    {
    }

    void Text::set_word_wrap(bool word_wrap)
{
    wordWrap = word_wrap;
}

void Text::set_max_width(float max_width)
{
    maxWidth = max_width;
}

void Text::setString(const sf::String &text)
{
   if( wordWrap )
       TextManipulators::wordwrap(*this, maxWidth);
   else
        sf::Text::setString(text);
}
} // namespace ml

