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
        } else if constexpr (std::is_default_constructible_v<T>) {
            return T();
        } else {
            // T has no default constructor (e.g. sf::Sprite in SFML 3).
            // Graphic<T>() must not be called for these types — use Graphic(const T&).
            throw std::logic_error(
                "Graphic<T>() called for a type with no default constructor. "
                "Construct with a valid T instance instead.");
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