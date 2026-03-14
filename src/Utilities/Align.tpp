//
// Created by Dave Smith on 3/8/26.
//

#ifndef ALIGN_TPP
#define ALIGN_TPP
#include <Malena/Utilities/Align.h>
namespace ml
{
    template<class T>
	void Align::centerText(const T &obj, sf::Text &text)
	{
		// Get the LOCAL bounds of the text (unaffected by transformations)
		sf::FloatRect textLocalBounds = text.getLocalBounds();

		// Set the origin to the LOCAL center of the text
		text.setOrigin({
			textLocalBounds.position.x + textLocalBounds.size.x / 2.0f,
			textLocalBounds.position.y + textLocalBounds.size.y / 2.0f
		});

		// Get the GLOBAL bounds of the shape (includes transformations)
		sf::FloatRect shapeGlobalBounds = obj.getGlobalBounds();

		// Position text at the global center of the shape
		text.setPosition({
			shapeGlobalBounds.position.x + shapeGlobalBounds.size.x / 2.0f,
			shapeGlobalBounds.position.y + shapeGlobalBounds.size.y / 2.0f
		});
	}

	template<class S>
	void Align::setRightOf(const sf::FloatRect &bounds, S &obj, float spacing)
	{
		obj.setPosition({bounds.position.x + bounds.size.x + spacing, bounds.position.y});
	}

	template<class S>
	void Align::setLeftOf(const sf::FloatRect &bounds, S &obj, float spacing)
	{
		sf::FloatRect me = obj.getGlobalBounds();
		obj.setPosition({bounds.position.x - me.size.x - spacing, bounds.position.y});
	}

	template<class S>
	void Align::setBelow(const sf::FloatRect &bounds, S &obj, float spacing)
	{
		obj.setPosition({bounds.position.x, bounds.position.y + bounds.size.y + spacing});
	}

	template<class S>
	void Align::setAbove(const sf::FloatRect &bounds, S &obj, float spacing)
	{
		sf::FloatRect me = obj.getGlobalBounds();
		obj.setPosition({bounds.position.x, bounds.position.y - me.size.y - spacing});
	}

	template<class S>
	void Align::centerOn(const sf::FloatRect &bounds, S &obj)
	{
		sf::FloatRect me = obj.getGlobalBounds();
		obj.setPosition({bounds.position.x + bounds.size.x / 2.0f - me.size.x / 2.0f,
						 bounds.position.y + bounds.size.y / 2.0f - me.size.y / 2.0f});
	}

	template<class S>
	void Align::centerVertically(const sf::FloatRect &bounds, S &obj)
	{
		float x = obj.getPosition().x;
		centerOn(bounds, obj);
		obj.setPosition({x, obj.getPosition().y});
	}

	template<class S>
	void Align::centerHorizontally(const sf::FloatRect &bounds, S &obj)
	{
		float y = obj.getPosition().y;
		centerOn(bounds, obj);
		obj.setPosition({obj.getPosition().x, y});
	}

	template<class T, class S>
	void Align::setRightOf(const T &constObj, S &obj, float spacing)
	{
		setRightOf(constObj.getGlobalBounds(), obj, spacing);
	}

	template<class T, class S>
	void Align::setLeftOf(const T &constObj, S &obj, float spacing)
	{
		setLeftOf(constObj.getGlobalBounds(), obj, spacing);
	}

	template<class T, class S>
	void Align::setBelow(const T &constObj, S &obj, float spacing)
	{
		setBelow(constObj.getGlobalBounds(), obj, spacing);
	}

	template<class T, class S>
	void Align::setAbove(const T &constObj, S &obj, float spacing)
	{
		top(constObj.getGlobalBounds(), obj, spacing);
	}

	template<class T, class S>
	void Align::centerOn(const T &constObj, S &obj)
	{
		centerOn(constObj.getGlobalBounds(), obj);
	}

	template<class T, class S>
	void Align::centerVertically(const T &constObj, S &obj)
	{
		centerVertically(constObj.getGlobalBounds(), obj);
	}

	template<class T, class S>
	void Align::centerHorizontally(const T &constObj, S &obj)
	{
		centerHorizontally(constObj.getGlobalBounds(), obj);
	}
	template<class S, class U>
	void Align::setRightOf(const sf::Vector2<U> &size, S &obj, float spacing)
	{
		sf::FloatRect bounds {{0.f,0.f}, (sf::Vector2f)size};
		setRightOf(bounds, obj, spacing);
	}

	template<class S, class U>
	void Align::setLeftOf(const sf::Vector2<U> &size, S &obj, float spacing)
	{
		sf::FloatRect bounds {{0.f,0.f}, (sf::Vector2f)size};
		setLeftOf(bounds, obj, spacing);
	}

	template<class S, class U>
	void Align::setBelow(const sf::Vector2<U> &size, S &obj, float spacing)
	{
		sf::FloatRect bounds {{0.f,0.f}, (sf::Vector2f)size};
		bottom(bounds, obj, spacing);
	}

	template<class S, class U>
	void Align::setAbove(const sf::Vector2<U> &size, S &obj, float spacing)
	{
		sf::FloatRect bounds {{0.f,0.f}, (sf::Vector2f)size};
		top(bounds, obj, spacing);
	}

	template<class S, class U>
	void Align::centerOn(const sf::Vector2<U> &size, S &obj)
	{
		sf::FloatRect bounds {{0.f,0.f}, (sf::Vector2f)size};
		centerOn(bounds, obj);
	}

	template<class S, class U>
	void Align::centerVertically(const sf::Vector2<U> &size, S &obj)
	{
		sf::FloatRect bounds {{0.f,0.f}, (sf::Vector2f)size};
		centerVertically(bounds, obj);
	}

	template<class S, class U>
	void Align::centerHorizontally(const sf::Vector2<U> &size, S &obj)
	{
		sf::FloatRect bounds {{0.f,0.f}, (sf::Vector2f)size};
		centerHorizontally(bounds, obj);
	}
} //namespace ml
#endif