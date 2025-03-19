//
// Created by Dave Smith on 11/13/22.
//

#ifndef PORTFOLIO_POSITIONABLE_H
#define PORTFOLIO_POSITIONABLE_H
#include <SFML/Graphics.hpp>
#include "../Utilities/Tween.h"
#include <cmath>
#include <queue>
#include "../Utilities/Helper.h"

namespace ml {


class Positionable
{
private:
    float velocityX, velocityY, framerate = 60.f;
    sf::Vector2f initialPosition = {MAXFLOAT, MAXFLOAT};
    sf::Clock clock;
    std::queue<sf::Vector2f> points;
    bool scrolling = false;

    bool error(sf::Vector2f a, sf::Vector2f b, float err = 0.1f);
    void generatePoints(sf::Vector2f position, float duration, Tween tween);
    void generateExponential(sf::Vector2f position, float duration);



public:
    void calcVelocity(sf::Vector2f velocity, float seconds);
    void setFramerate(float framerate);
    void moveTo(sf::Vector2f position, float seconds=1.f);
    void moveTo(sf::FloatRect position, float seconds=1.f);
    void moveDistance(sf::Vector2f distance, float seconds=1.f);
    bool isScrolling();
    virtual void setPosition(const sf::Vector2f& position) = 0;
    virtual sf::Vector2f getPosition() const = 0;
    //void moveTo(sf::Vector2f position, float duration, Tween tween=LINEAR);
    template<class T>
    void right(T& obj, float spacing = 0);
    template<class T>
    void left(T& obj, float spacing = 0);
    template<class T>
    void bottom(T& obj, float spacing = 0);
    template<class T>
    void top(T& obj, float spacing = 0);
    template<class T>
    void center(T& obj);
    void centerText(sf::Text& obj);
    template<class T>
    static void centerText(const T& obj, sf::Text& text);
    template<class T, class S>
    static void right(const T& constObj, S& obj, float spacing = 0);
    template<class T, class S>
    static void left(const T& constObj, S& obj, float spacing = 0);
    template<class T, class S>
    static void bottom(const T& constObj, S& obj, float spacing = 0);
    template<class T, class S>
    static void top(const T& constObj, S& obj, float spacing = 0);
    template<class T, class S>
    static void center(const T& constObj, S& obj);
    template<class T, class S>
    static void yCenter(const T& constObj, S& obj);
    template<class T, class S>
    static void xCenter(const T& constObj, S& obj);
    virtual sf::FloatRect getGlobalBounds() const = 0;
};


} // namespace ml
#include "PositionableTemplates.cpp"
#endif //PORTFOLIO_POSITIONABLE_H


