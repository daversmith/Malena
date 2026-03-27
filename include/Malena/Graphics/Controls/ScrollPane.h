#ifndef SCROLLPANE_H
#define SCROLLPANE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <Malena/Core/Component.h>
#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Manifests/ManifestResources.h>
#include "ScrollPaneManifest.h"

namespace ml
{
	class ScrollPane : public ml::Component<ScrollPaneManifest>
	{
	public:
		using App   = ml::ManifestResources<ScrollPaneManifest>;
		using State = ScrollPaneManifest::State;
		using Flag  = ScrollPaneManifest::Flag;

		ScrollPane(float width, float height);

		void addComponent(ml::Core& component);
		void removeComponent(ml::Core& component);

		// Styling
		void setBackgroundColor(sf::Color color);
		void setScrollBarColor(sf::Color color);
		void setScrollBarWidth(float width);

		void setPosition(const sf::Vector2f& position) override;
		sf::Vector2f getPosition() const override;
		sf::FloatRect getGlobalBounds() const override;
		sf::RenderStates getRenderStates() const override;

	protected:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	private:
		float _width;
		float _height;
		float _scrollOffsetY  = 0.f;
		float _scrollOffsetX  = 0.f;
		float _scrollSpeed    = 20.f;
		float _scrollBarWidth = 8.f;

		sf::Vector2f _position = {0.f, 0.f};

		sf::RectangleShape _background;
		mutable sf::RectangleShape _scrollBarTrack;
		mutable ml::Rectangle              _scrollBarThumb;  // ← ml::Rectangle with Draggable

		mutable sf::RenderTexture  _renderTexture;
		std::vector<ml::Core*>     _children;

		sf::Color _scrollBarTrackColor = sf::Color(200, 200, 200, 150);
		sf::Color _scrollBarThumbColor = sf::Color(100, 100, 100, 200);

		void  stackChildren() const;
		float getTotalContentHeight() const;
		float getTotalContentWidth()  const;
		void  updateScrollBar()       const;
	};

}

#endif //SCROLLPANE_H