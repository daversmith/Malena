//
// Created by Dave Smith on 4/9/25.
//

#include <Malena/Utilities/TextureSlicer.h>

namespace ml
{
	ImageRects TextureSlicer::getImageRects(const sf::Texture &texture, int rows, int cols)
	{
		int width = texture.getSize().x / cols;
		int height = texture.getSize().y / rows;
		ImageRects image_rects(cols);
		for (int row=0; row<rows; ++row)
		{
			for (int col=0; col<cols; col++)
			{
				image_rects.push(sf::IntRect(
								{row * height, col * width},
								{width, height}));
			}
		}

		return image_rects;
	}
}

