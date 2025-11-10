//
// Created by Dave Smith on 11/9/25.
//

#ifndef ROUNDEDRECTANGLE_H
#define ROUNDEDRECTANGLE_H

#include <SFML/Graphics.hpp>
#include <cmath>

#include "Shape.h"
#include "Malena/Traits/Customizable.h"


namespace ml
{
    class RoundedRectangle : public sf::Shape {
    public:
        explicit RoundedRectangle(const sf::Vector2f& size = sf::Vector2f(0, 0),
                                  float radius = 0.f,
                                  unsigned int cornerPointCount = 8);

        void setSize(const sf::Vector2f& size);
        const sf::Vector2f& getSize() const;

        void setRadius(float radius);
        float getRadius() const;

        void setCornerPointCount(unsigned int count);
        unsigned int getCornerPointCount() const;

        virtual std::size_t getPointCount() const override;
        virtual sf::Vector2f getPoint(std::size_t index) const override;

    private:
        sf::Vector2f m_size;
        float m_radius;
        unsigned int m_cornerPointCount;
    };
}



#endif //ROUNDEDRECTANGLE_H
