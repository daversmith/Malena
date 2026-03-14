 #ifndef GRID_H
#define GRID_H

#pragma once

#include <SFML/Graphics.hpp>

#include <Malena/Engine/App/Application.h>
#include <Malena/Core/CoreManager.h>

namespace ml
{
	class Grid final : public CoreManager<Positionable>, public Core
	{
		float max_height = 0, max_width = 0, spacing = 10;
		unsigned int row = 1, col = 1;

		void reposition();

	public:
		Grid();

		Grid(sf::Vector2u size);

		[[nodiscard]] float getSpacing() const;

		void setSpacing(float spacing);

		[[nodiscard]] int getRow() const;

		void setRow(int row);

		void addComponent(Core &component);

		// void addToApp(Application &app) const;
		//
		// void addToApp(Controller &app) const;

	// protected:
	// 	void draw(sf::RenderTarget &target, sf::RenderStates states) const; //override;

	public:
		void setPosition(const sf::Vector2f &position) ;//override;

		[[nodiscard]] sf::Vector2f getPosition() const;// override;


		[[nodiscard]] sf::FloatRect getGlobalBounds() const;// override;
	};
} // namespace ml
#endif // GRID_H
