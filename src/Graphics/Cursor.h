
#ifndef CURSOR_H
#define CURSOR_H
#include "Text.h"
#include "../Managers/FontManager.h"

namespace ml
{
    class Cursor : public Text
    {
        inline static sf::Clock clock;
        using Text::setString;
    public:
        using Text::Text;
        Cursor(const sf::Font& font = FontManager::DEFAULT);
        void eventHandler(const std::optional<sf::Event> &event) override;
        void update() override;
    };
}





#endif //CURSOR_H
