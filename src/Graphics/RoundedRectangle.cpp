//
// Created by Dave Smith on 11/9/25.
//


#include <Malena/Graphics/RoundedRectangle.h>
#include <algorithm>
namespace ml
{


RoundedRectangle::RoundedRectangle(const sf::Vector2f& size, float radius, unsigned int cornerPointCount)
    : m_size(size)
    , m_radius(radius)
    , m_cornerPointCount(cornerPointCount)
{
    update();
}

void RoundedRectangle::setSize(const sf::Vector2f& size) {
    m_size = size;
    update();
}

const sf::Vector2f& RoundedRectangle::getSize() const {
    return m_size;
}

void RoundedRectangle::setRadius(float radius) {
    m_radius = radius;
    update();
}

float RoundedRectangle::getRadius() const {
    return m_radius;
}

void RoundedRectangle::setCornerPointCount(unsigned int count) {
    m_cornerPointCount = count;
    update();
}

unsigned int RoundedRectangle::getCornerPointCount() const {
    return m_cornerPointCount;
}

std::size_t RoundedRectangle::getPointCount() const {
    // If no rounding, just return 4 corners
    if (m_radius <= 0.f || m_cornerPointCount == 0) {
        return 4;
    }

    // Each corner has cornerPointCount+1 points
    return 4 * (m_cornerPointCount + 1);
}

sf::Vector2f RoundedRectangle::getPoint(std::size_t index) const {
    // Clamp radius to valid range
    float actualRadius = std::min(m_radius, std::min(m_size.x, m_size.y) / 2.f);

    // If no rounding, return rectangle corners
    if (actualRadius <= 0.f || m_cornerPointCount == 0) {
        switch (index) {
            case 0: return sf::Vector2f(0, 0);
            case 1: return sf::Vector2f(m_size.x, 0);
            case 2: return sf::Vector2f(m_size.x, m_size.y);
            case 3: return sf::Vector2f(0, m_size.y);
            default: return sf::Vector2f(0, 0);
        }
    }

    const float pi = 3.14159265358979323846f;
    const float deltaAngle = (pi / 2.f) / static_cast<float>(m_cornerPointCount);
    const std::size_t pointsPerCorner = m_cornerPointCount + 1;

    // Determine which corner this point belongs to
    std::size_t corner = index / pointsPerCorner;
    std::size_t pointInCorner = index % pointsPerCorner;

    float angle = 0.f;
    sf::Vector2f center;

    switch (corner) {
        case 0: // Top-left corner
            center = sf::Vector2f(actualRadius, actualRadius);
            angle = pi + (deltaAngle * pointInCorner);
            break;

        case 1: // Top-right corner
            center = sf::Vector2f(m_size.x - actualRadius, actualRadius);
            angle = (3.f * pi / 2.f) + (deltaAngle * pointInCorner);
            break;

        case 2: // Bottom-right corner
            center = sf::Vector2f(m_size.x - actualRadius, m_size.y - actualRadius);
            angle = (deltaAngle * pointInCorner);
            break;

        case 3: // Bottom-left corner
            center = sf::Vector2f(actualRadius, m_size.y - actualRadius);
            angle = (pi / 2.f) + (deltaAngle * pointInCorner);
            break;

        default:
            return sf::Vector2f(0, 0);
    }

    return sf::Vector2f(
        center.x + actualRadius * std::cos(angle),
        center.y + actualRadius * std::sin(angle)
    );
}


}