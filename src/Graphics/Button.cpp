//
// Created by Dave R. Smith on 3/9/25.
//
#ifndef BUTTON_CPP
#define BUTTON_CPP
#include "Button.h"


template <typename T, typename S>
Button<T, S>::Button(const sf::Font &font, std::optional<S> buttonSize, const std::string &text, unsigned int charSize)
  : Text(font, text, charSize), T()
{
  if (buttonSize.has_value()) {
    *static_cast<T *>(this) = T(buttonSize.value());  // Assign after construction
  }
  T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
}
template <typename T, typename S>
void Button<T, S>::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  T::draw(target, states);
  sf::Text t = *this;
  target.draw(t, states);
}
template <typename T, typename S>
void Button<T, S>::setPosition(const sf::Vector2f &position)
{
  T::setPosition(position);
  T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
}
template <typename T, typename S>
void Button<T, S>::setString(const sf::String &text)
{
  sf::Text::setString(text);
  T::centerText(*dynamic_cast<T *>(this), *dynamic_cast<sf::Text *>(this));
}

#endif