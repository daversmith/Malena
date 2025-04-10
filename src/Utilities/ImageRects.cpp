//
// Created by Dave Smith on 4/9/25.
//

#include <Malena/Utilities/ImageRects.h>

namespace ml {
	ImageRects::ImageRects(int cols)
		: _cols(cols)
	{
	}
	const sf::IntRect &ImageRects::getIntRect(int row, int col)
	{
		return _intRects.at(row * _cols + col);
	}
	void ImageRects::push(const sf::IntRect &int_rect)
	{
		_intRects.push_back(int_rect);
	}
} // ml