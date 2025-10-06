#ifndef UICOMPONENT_H
#define UICOMPONENT_H


#include <SFML/Graphics.hpp>

#include <iostream>
#include <Malena/Interfaces/UIComponent.h>
#include <Malena/Traits/CustomStateful.h>
namespace ml
{


	template<typename CustomStateEnum = void>
	class UIComponentWith : public virtual UIComponent, public CustomStateful<CustomStateEnum>{
	public:
		// Bring both enableState overloads into scope
		using UIComponent::enableState;
		using UIComponent::disableState;
		using UIComponent::setState;
		using UIComponent::toggleState;
		using UIComponent::checkState;

		using CustomStateful<CustomStateEnum>::enableState;
		using CustomStateful<CustomStateEnum>::disableState;
		using CustomStateful<CustomStateEnum>::setState;
		using CustomStateful<CustomStateEnum>::toggleState;
		using CustomStateful<CustomStateEnum>::checkState;
	};



} // namespace ml
#endif // UICOMPONENT_H
