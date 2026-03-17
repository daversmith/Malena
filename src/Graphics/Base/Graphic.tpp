//
// Created by Dave Smith on 3/6/26.
//

#include <Malena/Graphics/Base/Graphic.h>
#ifndef Graphic_TPP
#define Graphic_TPP
namespace ml
{
    template<typename T>
    Graphic<T>::operator const sf::Drawable&() const {
        return static_cast<const T&>(*this);
    }
    template<typename T>
     T Graphic<T>::isItText() {
        if constexpr (std::is_same_v<T, sf::Text>) {
            return sf::Text(FontManager<>::getDefault());
        } else {
            return T();
        }
    }
    template<typename T>
    Graphic<T>::Graphic() : T(isItText())
    {
    }

    template <typename T>
    template<typename U>
    Graphic<T>::Graphic(const U &obj) : T(obj)
    {
    }

    template<typename T>
    void Graphic<T>::draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        target.draw(static_cast<const T>(*this), states);
    }

    template<typename T>
    sf::FloatRect Graphic<T>::getGlobalBounds() const
    {
        return T::getGlobalBounds();
    }

    template<typename T>
    void Graphic<T>::setPosition(const sf::Vector2f &position)
    {
        T::setPosition(position);
    }

    template<typename T>
    sf::Vector2f Graphic<T>::getPosition() const
    {
        return T::getPosition();
    }
}//namespace ml
#endif