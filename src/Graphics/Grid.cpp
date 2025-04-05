
#include <Malena/Graphics/Grid.h>

namespace ml
{
	Grid::Grid()
	{
	}

	Grid::Grid(const sf::Vector2u size) : row(size.x), col(size.y)
	{
	}

	float Grid::getSpacing() const
	{
		return spacing;
	}

	void Grid::setSpacing(const float spacing)
	{
		this->spacing = spacing;
	}

	int Grid::getRow() const
	{
		return row;
	}

	void Grid::setRow(const int row)
	{
		this->row = row;
	}

	void Grid::addComponent(UIComponent &component)
	{
		const auto c = reinterpret_cast<sf::Shape *>(&component);
		const auto h = c->getLocalBounds().size.y;
		const auto w = c->getLocalBounds().size.x;
		max_height = h > max_height ? h : max_height;
		max_width = w > max_width ? w : max_width;
		ComponentsManager::addComponent(component);
		reposition();
	}

	void Grid::reposition()
	{
		const auto &components = getUIComponents(); // Store reference to avoid repeated calls
		if (components.empty())
			return; // Avoid accessing empty vector

		// Position first element (top-left)
		for (int i = 0; i < components.size(); ++i)
		{
			if (i >= col) // If it's in the second row or below, position below the element above
				components[i - col]->bottom(*components[i], spacing);

			if (i % col != 0) // If it's not the first column, position to the right of the previous element
				components[i - 1]->right(*components[i], spacing);
		}
	}

	void Grid::draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		for (auto &c : getUIComponents())
			target.draw(*c);
	}

	void Grid::setPosition(const sf::Vector2f &position)
	{
		if (!getUIComponents().empty())
			getUIComponents().front()->setPosition(position);
		reposition();
	}

	sf::Vector2f Grid::getPosition() const
	{
		if (!getUIComponents().empty())
			return getUIComponents().front()->getPosition();
		return {};
	}


	sf::FloatRect Grid::getGlobalBounds() const
	{
		// TODO fix gloabalBounds
		return {};
	}

	void Grid::addToApplication(Application &application)
	{
		for (auto &c : getUIComponents())
			application.addToApplication(*c);
	}

} // namespace ml
