
#ifndef SHAPE_H
#define SHAPE_H



#include <SFML/Graphics/RenderTarget.hpp>

#include "../Interfaces/UIComponent.h"
namespace ml {


template <typename T>
class Shape :
    public T,
    public UIComponent
{
public:
    using T::T;
// protected:
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const ;

public:
    Shape();
    Shape(const sf::Font& font);
    void update() override;

    void eventHandler(const std::optional<sf::Event> &event) override;
    // using  T::getGlobalBounds;
    sf::FloatRect  getGlobalBounds() const override;

    // void setPosition(const sf::Vector2f &position) override;

    void setPosition(const sf::Vector2f &position);

    sf::Vector2f getPosition() const override;
};
} // namespace ml
#include "Shape.cpp"

#endif //SHAPE_H


