//
// Created by Dave Smith on 1/23/26.
//

#include <Malena/Graphics/Controls/ListItem.h>
#include <Malena/Engine/App/Application.h>
#include <Malena/Utilities/TextManipulators.h>

namespace ml
{
	ListItem::ListItem() : ListItem("")
	{
	}

	ListItem::ListItem(const sf::Font &font, std::optional<sf::Vector2f> itemSize, const std::string &text,
	    unsigned int charSize, const sf::Texture *startIcon, const sf::Texture *endIcon)
	        : _middle(font, itemSize, text, charSize)
	{
	    setupIcons(startIcon, endIcon);
	    _middle.setFillColor(sf::Color::Transparent);
	    // setSize ({getNaturalWidth(charSize), static_cast<float>(charSize)+_padding});
	}

	ListItem::ListItem(const std::string &text, const sf::Texture *startIcon, const sf::Texture *endIcon)
	    : ListItem(ml::FontManager<>::getDefault(), std::nullopt, text, 24, startIcon, endIcon)
	{
	}

	ListItem::~ListItem()
	{
	    delete _startIcon;
	    delete _endIcon;
	}

	void ListItem::setupIcons(const sf::Texture *startIcon, const sf::Texture *endIcon)
	{
	    if (startIcon)
	    {
	        setStartIcon(*startIcon);
	    }

	    if (endIcon)
	    {
	        setEndIcon(*endIcon);
	    }

	}

	void ListItem::scaleIcon(ml::Sprite *icon, const sf::Vector2f& size)
	{
	    if (icon)
	    {
	        float scale = size.y * 0.8f / icon->getGlobalBounds().size.y;
	        icon->setScale({scale, scale});
	    }

	}

	void ListItem::setStartIcon(const sf::Texture &icon)
	{
	    _startIcon = new ml::Sprite(icon);
	    // scaleIcon(*_startIcon);
	    position();
	}

	void ListItem::setEndIcon(const sf::Texture &icon)
	{
	    _endIcon = new ml::Sprite(icon);
	    // scaleIcon(*_endIcon);
	    position();
	}

	void ListItem::setFillColor(const sf::Color &color)
	{
	    if (_startIcon)
	        _startIcon->setColor(color);
	    if (_endIcon)
	        _endIcon->setColor(color);
	    _middle.setFillColor(color);
	}

	void ListItem::setSize(const sf::Vector2f &size)
	{
	    sf::Vector2f scale = {0.f, 0.f};
	    _middle.setCharacterSize(size.y * 0.8f);

	    if (size.x < _middle.getSize().x)
	    {
	        auto text = ml::TextManipulators::wordwrap(
	            _middle.getString(),
	            _middle.getFont(),
	            _middle.getCharacterSize(),
	            size.x * 0.6f);

	        _middle.setString(text);

	        scale.x = _middle.getSize().x * 0.2f;
	        scale.y = _middle.getSize().y;
	    }
	    else
	    {
	        scale.x = size.x * .2f;
	        scale.y = size.y;
	        _middle.setSize({size.x * .6f, size.y});
	    }

	    scaleIcon(_startIcon, scale);
	    scaleIcon(_endIcon, scale);

	    position();
	}

	void ListItem::setCharacterSize(unsigned int size)
	{
	    _middle.setCharacterSize(size);
	}

	unsigned int ListItem::getCharacterSize() const
	{
	    return _middle.getCharacterSize();
	}

	std::string ListItem::getText() const
	{
	    return _middle.getString();
	}

	float ListItem::getIconsWidth() const
	{
	    float width = 0.0f;
	    if (_startIcon)
	        width += _startIcon->getGlobalBounds().size.x;
	    if (_endIcon)
	        width += _endIcon->getGlobalBounds().size.x;
	    return width;
	}
	// sf::Vector2f ListItem::getTextSize(unsigned int fontSize)
	// {
	//     // Create a temporary text object to measure
	//     sf::Text tempText(_middle.getFont());
	//     tempText.setString(_middle.getString());
	//     tempText.setCharacterSize(_middle.getCharacterSize());
	// }
	float ListItem::getNaturalWidth(unsigned int fontSize) const
	{
	    // Create a temporary text object to measure
	    sf::Text tempText(_middle.getFont());
	    tempText.setString(_middle.getString());
	    tempText.setCharacterSize(fontSize);

	    float textWidth = tempText.getGlobalBounds().size.x;
	    float iconsWidth = getIconsWidth();
	    return textWidth + iconsWidth + _padding;
	}

	void ListItem::setOnClick(std::function<void()> function)
	{
	    _onClick = function;
	}

	void ListItem::setPosition(const sf::Vector2f &position)
	{
	    if (_startIcon)
	        _startIcon->setPosition(position);
	    else
	        _middle.setPosition(position);

	    this->position();
	}

	sf::Vector2f ListItem::getPosition() const
	{
	    if (_startIcon)
	        return _startIcon->getPosition();
	    return _middle.getPosition();
	}

	sf::FloatRect ListItem::getGlobalBounds() const
	{
	    sf::FloatRect bounds = _middle.getGlobalBounds();
	    // ml::Helper::output(bounds);
	    if (_startIcon)
	    {
	        bounds.position = _startIcon->getPosition();
	        bounds.size.x += _startIcon->getGlobalBounds().size.x;
	    }
	    if (_endIcon)
	        bounds.size.x += _endIcon->getGlobalBounds().size.x;

	    return bounds;
	}

	void ListItem::draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
	    if (_startIcon)
	        target.draw(*_startIcon, states);
	    target.draw(_middle, states);
	    if (_endIcon)
	        target.draw(*_endIcon, states);
	}

	void ListItem::position()
	{
	    if (_startIcon)
	    {
	        sf::Vector2f middlePos = _startIcon->getPosition();
	        middlePos.x += _startIcon->getGlobalBounds().size.x;
	        _middle.setPosition(middlePos);  // always position middle next to icon
	    }

	    if (_endIcon)
	    {
	        sf::Vector2f endPos = _middle.getPosition();
	        endPos.x += _middle.getGlobalBounds().size.x;
	        _endIcon->setPosition(endPos);
	    }
	}

}

