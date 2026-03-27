//
// Created by Dave Smith on 1/23/26.
//

#ifndef LISTITEM_H
#define LISTITEM_H
#include <Malena/Graphics/Controls/ListComponent.h>
#include <Malena/Graphics/Controls/RectangleButton.h>
#include <Malena/Core/Core.h>
#include <Malena/Graphics/Sprites/Sprite.h>

namespace ml
{
	class ListItem : public ListComponent {
		ml::Sprite  *_startIcon = nullptr, *_endIcon = nullptr;
		ml::RectangleButton _middle;
		std::function<void()> _onClick;
		float _padding = 5.f;
		void position();
		void setupIcons(const sf::Texture* startIcon, const sf::Texture* endIcon);
		void scaleIcon(ml::Sprite* icon, const sf::Vector2f& size);
	public:
		ListItem();
		virtual ~ListItem();
		ListItem(const std::string &text, const sf::Texture *startIcon = nullptr, const sf::Texture *endIcon = nullptr);
		ListItem(const sf::Font &font = ml::FontManager<>::getDefault(),
				   std::optional<sf::Vector2f> itemSize = std::nullopt, const std::string &text = "",
				   unsigned int charSize = 30, const sf::Texture* startIcon = nullptr, const sf::Texture* endIcon = nullptr);

		void setPosition(const sf::Vector2f &position) override;
		sf::Vector2f getPosition() const override;
		sf::FloatRect getGlobalBounds() const override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

		void setStartIcon(const sf::Texture& icon);
		void setEndIcon(const sf::Texture& icon);
		void setFillColor(const sf::Color& color);
		void setSize(const sf::Vector2f& size);
		void setOnClick(std::function<void()>);

		// New methods for auto-sizing
		float getNaturalWidth(unsigned int fontSize) const;
		void setCharacterSize(unsigned int size);
		unsigned int getCharacterSize() const;
		std::string getText() const;
		float getIconsWidth() const;

		friend class List;
	};
}


#endif //LISTITEM_H