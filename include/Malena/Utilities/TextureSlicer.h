//
// Created by Dave Smith on 4/9/25.
//

#ifndef TEXTURESLICER_H
#define TEXTURESLICER_H

#include <SFML/Graphics.hpp>
#include "ImageRects.h"
namespace ml
{
	class TextureSlicer
	{
	public:
		static ml::ImageRects getImageRects(const sf::Texture& texture, int rows, int cols);
	};
}


#endif //TEXTURESLICER_H
