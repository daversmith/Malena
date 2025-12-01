//
// Created by Dave Smith on 11/13/22.
//


#include <iostream>

#include <Malena/Traits/Positionable.h>


namespace ml
{

	void Positionable::generatePoints(sf::Vector2f position, float duration, Tween tween)
	{
		switch (tween)
		{
			case EXPONENTIAL:
				if (points.empty())
					generateExponential(position, duration);
				else
					return;
		}
	}

	void Positionable::generateExponential(sf::Vector2f position, float duration)
	{
		float x = velocityX, y = 0.0;
		std::cout << "abs: " << std::abs(velocityX * points.size()) << std::endl;
		std::cout << "size: " << points.size() << std::endl;
		float count{};
		do
		{
			x += velocityX;
			y = (0.001f * std::pow(x, 2.f));
			points.emplace(velocityX, y);
			count += y;
		} while (count < 720);
	}

	void Positionable::centerText(sf::Text &obj)
	{
		centerText(*this, obj);
	}

	void Positionable::moveTo(sf::Vector2f position, float seconds)
	{
		if (seconds == 0)
		{
			setPosition(position);
			return;
		}

		if (!scrolling && !error(getPosition(), position, 1.f))
		{
			calcVelocity(position, seconds);
		}
		else if (scrolling && !error(getPosition(), position, 1.f))
		{
			sf::Vector2f v = getPosition();
			setPosition({v.x + velocityX, v.y + velocityY});
		}
		else
		{
			setPosition(position);
			scrolling = false;
			initialPosition = sf::Vector2f(FLT_MAX, FLT_MAX);
		}
	}

	void Positionable::moveDistance(sf::Vector2f distance, float seconds)
	{
		if (initialPosition == sf::Vector2f(FLT_MAX, FLT_MAX))
		{
			initialPosition = {getPosition().x + distance.x, getPosition().y + distance.y};
		}
		moveTo(initialPosition, seconds);
	}

	void Positionable::calcVelocity(sf::Vector2f position, float seconds)
	{
		if (!scrolling)
		{
			velocityX = (position.x - getPosition().x) / seconds / framerate;
			velocityY = (position.y - getPosition().y) / seconds / framerate;
			scrolling = true;
		}
	}

	bool Positionable::error(sf::Vector2f a, sf::Vector2f b, float err)
	{
		return abs( a.x - b.x) <= err && abs(a.y - b.y) <= err;
	}

	void Positionable::moveTo(sf::FloatRect position, float seconds)
	{
		moveTo(position.position, seconds);
	}

	void Positionable::setFramerate(float framerate)
	{
		Positionable::framerate = framerate;
	}

	bool Positionable::isScrolling()
	{
		return scrolling;
	}


} // namespace ml
