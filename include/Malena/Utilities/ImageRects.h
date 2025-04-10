//
// Created by Dave Smith on 4/9/25.
//

#ifndef IMAGERECTS_H
#define IMAGERECTS_H
#include <SFML/Graphics/Rect.hpp>
#include <vector>
namespace ml {

class ImageRects {
private:
	std::vector<sf::IntRect> _intRects;
	int _rows, _cols;
public:
	ImageRects(int cols);
	const sf::IntRect& getIntRect(int row, int col);
	void push(const sf::IntRect& int_rect);
};

} // ml

#endif //IMAGERECTS_H
