//
// Created by Dave Smith on 11/13/22.
//

#include <Malena/Utilities/Helper.h>
#include <iostream>
namespace ml
{

	void Helper::output(sf::Vector2f vector)
	{
		std::cout << "x: " << vector.x << ", y: " << vector.y << std::endl;
	}

	void Helper::output(sf::FloatRect floatRect)
	{
		// TODO: revisit
		 std::cout << "left: " << floatRect.position.x << ", top: " << floatRect.position.y
		<< ", width: " << floatRect.size.x << "height: " << floatRect.size.y << std::endl;
	}
} // namespace ml
