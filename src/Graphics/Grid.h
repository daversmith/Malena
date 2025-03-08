//
// Created by Dave Smith on 3/8/25.
//

#ifndef GRID_H
#define GRID_H
#include <vector>

#include "../Component.h"
#include <SFML/Graphics.hpp>

#include "../ComponentsManager.h"

class Grid  : public ComponentsManager, public UIComponent
{
    float max_height = 0, max_width = 0, spacing = 10;
    int row = 1, col = 1;

    void reposition();

public:
    Grid();
    Grid(sf::Vector2u size);
    [[nodiscard]] float getSpacing() const;

    void setSpacing(float spacing);

    [[nodiscard]] int getRow() const;

    void setRow(int row);

    void addComponent(UIComponent & component);

protected:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

public:
    void setPosition(const sf::Vector2f &position) override;

    sf::Vector2f getPosition() const override;

    void update() override;

    void eventHandler() override;

    [[nodiscard]] sf::FloatRect getGlobalBounds() const override;
};



#endif //GRID_H
