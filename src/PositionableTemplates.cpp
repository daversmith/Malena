//
// Created by Dave Smith on 11/13/22.
//

#ifndef PORTFOLIO_POSITION_CPP
#define PORTFOLIO_POSITION_CPP
#include "Positionable.h"

template<class T>
void Positionable::right(T &obj, float spacing)
{
    right(*this, obj, spacing);
}

template<class T>
void Positionable::left(T &obj, float spacing)
{
    left(*this, obj, spacing);
}

template<class T>
void Positionable::bottom(T &obj, float spacing)
{
    bottom( *this, obj,spacing);
}

template<class T>
void Positionable::top(T &obj, float spacing)
{
    top(*this,obj,  spacing);
}

template<class T, class S>
void Positionable::right(const T& constObj, S &obj, float spacing)
{
    sf::FloatRect ob = constObj.getGlobalBounds();
    obj.setPosition({ob.position.x + ob.size.x + spacing, ob.position.y});
}

template<class T, class S>
void Positionable::left(const T& constObj, S &obj, float spacing)
{
    sf::FloatRect ob = constObj.getGlobalBounds();
    sf::FloatRect me = obj.getGlobalBounds();
    obj.setPosition({ob.position.x - me.size.x - spacing,  ob.position.y});
}

template<class T, class S>
void Positionable::bottom(const T& constObj, S &obj, float spacing)
{
    sf::FloatRect ob = constObj.getGlobalBounds();
    obj.setPosition({ob.position.x,  ob.position.y + ob.size.y + spacing});
}

template<class T, class S>
void Positionable::top(const T& constObj, S &obj, float spacing)
{
    sf::FloatRect ob = constObj.getGlobalBounds();
    sf::FloatRect me = obj.getGlobalBounds();
    obj.setPosition({ob.position.x,  ob.position.y -  me.size.y - spacing});
}

template<class T, class S>
void Positionable::center(const T& constObj, S &obj)
{
    sf::FloatRect ob = constObj.getGlobalBounds();
    sf::FloatRect me = obj.getGlobalBounds();
    obj.setPosition({ob.position.x + ob.size.x/2.0f - me.size.x/2.0f,
                      ob.position.y +  ob.size.y/2.0f -  me.size.y/2.0f});
}

template<class T>
void Positionable::center(T &obj)
{
    center(obj, *this);
}

template<class T>
void Positionable::centerText(const T &obj, sf::Text &text)
{
    sf::FloatRect textRect = text.getGlobalBounds();
    sf::FloatRect tRect = obj.getGlobalBounds();
    sf::Vector2f center = {textRect.size.x/2.0f, textRect.size.y/2.f};
    sf::Vector2f localBounds = {center.x + text.getLocalBounds().position.x, center.y + text.getLocalBounds().position.y};
    sf::Vector2f rounded = {std::round(localBounds.x), std::round(localBounds.y)};
    text.setOrigin(rounded);
    text.setPosition({tRect.position.x + tRect.size.x/2, tRect.position.y + tRect.size.y/2});
}


template<class T, class S>
void Positionable::yCenter(const T &constObj, S &obj)
{
    float x = obj.getPosition().x;
    center(constObj, obj);
    obj.setPosition({x, obj.getPosition().y});

}

template<class T, class S>
void Positionable::xCenter(const T &constObj, S &obj)
{
    float y = obj.getPosition().y;
    center(constObj, obj);
    obj.setPosition({obj.getPosition().x, y});
}
#endif