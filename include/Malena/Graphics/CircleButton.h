
#ifndef CIRCLEBUTTON_H
#define CIRCLEBUTTON_H

#pragma once

#include "Button.h"
#include "Circle.h"


namespace ml
{
	class CircleButton : public Button<Circle, float>
	{
		using Button::Button;
	};

} // namespace ml

#endif // CIRCLEBUTTON_H
