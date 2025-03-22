//
// Created by Dave Smith on 3/8/25.
//

#ifndef UPDATEABLE_H
#define UPDATEABLE_H
#include <SFML/Graphics.hpp>
#include <optional>
namespace ml
{

	class Updateable
	{
	public:
		virtual ~Updateable() = default;

		virtual void update() = 0;
	};
} // namespace ml
#endif // UPDATEABLE_H
