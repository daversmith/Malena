#ifndef POSITIONABLE_TPP
#define POSITIONABLE_TPP
#include <Malena/Traits/Positionable.h>
namespace ml
{

	template<class T>
		void Positionable::setRightOf(T &obj, float spacing)
	{
		Align::setRightOf(obj, *this, spacing);
	}

	template<class T>
	void Positionable::setLeftOf(T &obj, float spacing)
	{
		Align::setLeftOf(obj, *this, spacing);
	}

	template<class T>
	void Positionable::setBelow(T &obj, float spacing)
	{
		Align::setBelow(obj, *this, spacing);
	}

	template<class T>
	void Positionable::setAbove(T &obj, float spacing)
	{
		Align::setAbove(obj, *this, spacing);
	}

	template<class T>
	void Positionable::center(T &obj)
	{
		Align::centerOn(obj, *this);
	}

}
#endif