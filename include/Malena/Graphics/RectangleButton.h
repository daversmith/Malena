
#ifndef RECTANGLEBUTTON_H
#define RECTANGLEBUTTON_H

#pragma once

#include "Button.h"
#include "Rectangle.h"

namespace ml
{
	class RectangleButton : public Button<Rectangle, sf::Vector2f>
	{
	public:
		using Button::Button;
		using sf::RectangleShape::setFillColor;
	};

} // namespace ml

#endif // RECTANGLEBUTTON_H
