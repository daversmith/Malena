#ifndef UICOMPONENTADAPTER_H
#define UICOMPONENTADAPTER_H

#pragma once

#include <Malena/Interfaces/UIComponent.h>

namespace ml
{
	class UIComponentAdapter : public UIComponent
	{
	protected:
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	public:
		void update();

		void eventHandler(const std::optional<sf::Event> &event);

		[[nodiscard]] sf::FloatRect getGlobalBounds() const override;

		void setPosition(const sf::Vector2f &position) override;

		[[nodiscard]] sf::Vector2f getPosition() const override;
	};
} // namespace ml

#endif // UICOMPONENTADAPTER_H
