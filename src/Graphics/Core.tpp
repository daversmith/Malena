//
// Created by Dave Smith on 3/6/26.
//

#include <Malena/Graphics/Core.h>
#ifndef CORE_CPP
#define CORE_CPP
namespace ml
{
    template<typename T>
    Core<T>::operator const sf::Drawable&() const {
        return static_cast<const T&>(*this);
    }
    template<typename T>
     T Core<T>::isItText() {
        if constexpr (std::is_same_v<T, sf::Text>) {
            return sf::Text(FontManager<>::getDefault());
        } else {
            return T();
        }
    }
    template<typename T>
    Core<T>::Core() : T(isItText())
    {
    }

    template <typename T>
    template<typename U>
    Core<T>::Core(const U &obj) : T(obj)
    {
    }

    template<typename T>
    void Core<T>::draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        target.draw(static_cast<const T>(*this), states);
    }

    template<typename T>
    sf::FloatRect Core<T>::getGlobalBounds() const
    {
        return T::getGlobalBounds();
    }

    template<typename T>
    void Core<T>::setPosition(const sf::Vector2f &position)
    {
        T::setPosition(position);
    }

    template<typename T>
    sf::Vector2f Core<T>::getPosition() const
    {
        return T::getPosition();
    }
}//namespace ml
#endif