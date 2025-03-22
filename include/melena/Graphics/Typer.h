
#ifndef TYPER_H
#define TYPER_H
#include "Shape.h"
#include "../Managers/FontManager.h"

namespace ml
{
    class Typer : public virtual ml::Shape<sf::Text>
    {
        void registerEvents();
    public:
        using Shape::Shape;
        Typer(const sf::Font& font = ml::FontManager::DEFAULT);
    };
}




#endif //TYPER_H
