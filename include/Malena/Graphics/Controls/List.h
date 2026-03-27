//
// Created by Dave Smith on 1/23/26.
//

#ifndef LIST_H
#define LIST_H

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "ListComponent.h"
#include <Malena/Core/Core.h>

namespace ml
{
	class List : public ListComponent {
	private:
		std::vector<ListComponent*> _items;
		// std::vector<std::unique_ptr<ListComponent>> _items;
		std::vector<sf::RectangleShape> _dividers;

		sf::Vector2f _position;
		float _indentAmount = 20.0f;
		float _currentIndent = 0.0f;
		bool _showDividers = true;
		sf::Color _dividerColor = sf::Color(200, 200, 200);
		float _dividerThickness = 1.0f;

		void repositionItems();
		void updateDividers();

	public:
		List();
		virtual ~List() = default;

		// Item management
		void addItem(ListComponent* item);
		// void addItem(std::unique_ptr<ListComponent> item);

		// Positioning
		void setPosition(const sf::Vector2f &position) override;
		sf::Vector2f getPosition() const override;
		sf::FloatRect getGlobalBounds() const override;

		// List-specific settings
		void setIndentAmount(float amount);
		float getIndentAmount() const { return _indentAmount; }
		void setCurrentIndent(float indent) { _currentIndent = indent; }

		// Dividers
		void setShowDividers(bool show);
		bool getShowDividers() const { return _showDividers; }
		void setDividerColor(const sf::Color& color);
		void setDividerThickness(float thickness);

		// Drawing
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

		// Utility
		void autoSizeAllItems(unsigned int baseFontSize = 30);
		float getTotalHeight() const;
		float getMaxWidth() const;
		size_t getItemCount() const { return _items.size(); }
		void clear();

	private:
		// void addToApplication(ml::Application &application) override;
	};
}


#endif //LIST_H