#ifndef UICOMPONENTADAPTER_H
#define UICOMPONENTADAPTER_H

#pragma once

#include <Malena/Interfaces/UIComponent.h>

namespace ml
{
	class UIComponentAdapter : public UIComponent
	{
	protected:
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const ;//override;

	public:
		virtual void update();

		virtual void eventHandler(const std::optional<sf::Event> &event);

		virtual  sf::FloatRect getGlobalBounds() const override;

		virtual void setPosition(const sf::Vector2f &position) override;

		virtual sf::Vector2f getPosition() const override;
	};
} // namespace ml

#endif // UICOMPONENTADAPTER_H
