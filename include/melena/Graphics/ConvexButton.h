
#ifndef CONVEXBUTTON_H
#define CONVEXBUTTON_H

#include "Button.h"
#include "Convex.h"


namespace ml
{
	class ConvexButton : public Button<Convex, std::size_t>
	{
		using Button::Button;
	};

} // namespace ml
#endif // CONVEXBUTTON_H
