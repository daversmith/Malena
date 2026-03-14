
#ifndef CONVEXBUTTON_H
#define CONVEXBUTTON_H

#pragma once

#include <Malena/Graphics/Button.h>
#include <Malena/Graphics/Convex.h>
#include <Malena/Traits/Customizable.h>

namespace ml
{
	class ConvexButton : public Button<Convex, std::size_t>
	{
		using Button::Button;
	};

	template <typename MANIFEST>
	class ConvexButtonWith: public virtual Customizable<ConvexButton, MANIFEST>{};

} // namespace ml
#endif // CONVEXBUTTON_H
