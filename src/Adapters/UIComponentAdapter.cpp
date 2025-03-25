#include <Malena/Adapters/UIComponentAdapter.h>

namespace ml
{

	void UIComponentAdapter::draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
	}

	void UIComponentAdapter::update()
	{
	}

	void UIComponentAdapter::eventHandler(const std::optional<sf::Event> &event)
	{
	}

	sf::FloatRect UIComponentAdapter::getGlobalBounds() const
	{
		return sf::FloatRect();
	}

	void UIComponentAdapter::setPosition(const sf::Vector2f &position)
	{
	}

	sf::Vector2f UIComponentAdapter::getPosition() const
	{
		return sf::Vector2f();
	}
} // namespace ml
