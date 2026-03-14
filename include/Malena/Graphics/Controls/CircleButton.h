
#ifndef CIRCLEBUTTON_H
#define CIRCLEBUTTON_H

#pragma once

#include <Malena/Graphics/Base/Button.h>
#include <Malena/Graphics/Primitives/Circle.h>


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
