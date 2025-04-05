//
// Created by Dave R. Smith on 3/5/25.
//

#include <Malena/Adapters/UIComponentAdapter.h>
#include <Malena/Application/Application.h>

namespace ml
{
	UIComponent::UIComponent(sf::RenderWindow &window) : window(&window)
	{
		onClick([]() {});
		onHover([]() {});
	}

	UIComponent::~UIComponent()
	{
		unsubscribeAll();
	}

	void UIComponent::addToApplication(Application& application)
	{
		application.addToApplication(*this);
	}
} // namespace ml
