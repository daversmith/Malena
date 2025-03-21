
#ifndef TYPER_H
#define TYPER_H
#include "Shape.h"
#include "../Managers/FontManager.h"

namespace ml
{
    class Typer : public virtual ml::Shape<sf::Text>
    {
    public:
        using Shape::Shape;
        Typer(const sf::Font& font = ml::FontManager::DEFAULT);
        void eventHandler(const std::optional<sf::Event> &event) override;
        void update() override;
    };
}




#endif //TYPER_H
