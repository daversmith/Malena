
#ifndef CIRCLEBUTTON_H
#define CIRCLEBUTTON_H

#pragma once

#include <Malena/Graphics/Button.h>
#include <Malena/Graphics/Circle.h>


namespace ml
{
	class CircleButton : public Button<Circle, float>
	{
		using Button::Button;
	};

	template <typename MANIFEST>
	class CircleButtonWith: public virtual Customizable<CircleButton, MANIFEST>{};


} // namespace ml

#endif // CIRCLEBUTTON_H
