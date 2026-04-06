// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#include <Malena/Graphics/Layouts/Grid.h>
#include <SFML/Graphics/Shape.hpp>

namespace ml
{
    Grid::Grid() {}

    Grid::Grid(const sf::Vector2u size) : row(size.x), col(size.y) {}

    float Grid::getSpacing() const { return spacing; }

    void Grid::setSpacing(const float s)
    {
        spacing = s;
        reposition();
    }

    int Grid::getRow() const { return row; }

    void Grid::setRow(const int r)
    {
        row = r;
        reposition();
    }

    void Grid::addItem(Core& item)
    {
        // Measure bounds via sf::Shape — matches prior behaviour
        const auto* shape = dynamic_cast<const sf::Shape*>(&item);
        if (shape)
        {
            const auto h = shape->getLocalBounds().size.y;
            const auto w = shape->getLocalBounds().size.x;
            if (h > max_height) max_height = h;
            if (w > max_width)  max_width  = w;
        }
        _items.push_back(&item);
        reposition();
    }

    void Grid::reposition()
    {
        if (_items.empty()) return;

        for (int i = 0; i < static_cast<int>(_items.size()); ++i)
        {
            if (i >= static_cast<int>(col))
                _items[i - col]->setBelow(*_items[i], spacing);

            if (i % static_cast<int>(col) != 0)
                _items[i - 1]->setRightOf(*_items[i], spacing);
        }
    }

    void Grid::setPosition(const sf::Vector2f& position)
    {
        if (!_items.empty())
            _items.front()->setPosition(position);
        reposition();
    }

    sf::Vector2f Grid::getPosition() const
    {
        if (!_items.empty())
            return _items.front()->getPosition();
        return {};
    }

    sf::FloatRect Grid::getGlobalBounds() const
    {
        // TODO: compute proper bounding rect
        return {};
    }

} // namespace ml
