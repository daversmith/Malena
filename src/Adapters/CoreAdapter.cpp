#include <Malena/Adapters/CoreAdapter.h>

namespace ml
{

	void CoreAdapter::draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
	}

	void CoreAdapter::update()
	{
	}

	void CoreAdapter::eventHandler(const std::optional<sf::Event> &event)
	{
	}

	sf::FloatRect CoreAdapter::getGlobalBounds() const
	{
		return sf::FloatRect();
	}

	void CoreAdapter::setPosition(const sf::Vector2f &position)
	{
	}

	sf::Vector2f CoreAdapter::getPosition() const
	{
		return sf::Vector2f();
	}
} // namespace ml
