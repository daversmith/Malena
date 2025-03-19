//
// Created by Dave R. Smith on 3/9/25.
//

#ifndef BUTTON_H
#define BUTTON_H

#include "../Managers/FontManager.h"
#include <SFML/Graphics/Text.hpp>
#include "../Interfaces/UIComponent.h"

namespace ml {

template <typename T, typename S>
class Button : public  T, public sf::Text//, public virtual UIComponent
{
  static_assert(std::is_base_of_v<UIComponent, T>, "T must be derived from UIComponent");

public:
  Button(const sf::Font& font = FontManager::getFont(FontManager::ARIAL), std::optional<S> buttonSize = std::nullopt, const std::string& text = "", unsigned int charSize = 30);
  void setPosition(const sf::Vector2f& position) override;
  void setString(const sf::String& text);

protected:
  void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};

} // namespace ml
#include "Button.cpp"
#endif //BUTTON_H


