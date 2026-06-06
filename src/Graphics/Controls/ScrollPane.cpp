#include <Malena/Graphics/Controls/ScrollPane.h>
#include <Malena/Utilities/ClipView.h>

namespace ml
{
	ScrollPane::ScrollPane(float width, float height)
    : _width(width), _height(height)
	{
	    _background.setSize({width, height});
	    _background.setFillColor(sf::Color(40, 40, 40, 200));

	    _scrollBarTrack.setFillColor(_scrollBarTrackColor);

	    // Set up scrollbar thumb as a draggable Rectangle locked to Y axis
	    _scrollBarThumb.setFillColor(_scrollBarThumbColor);
	    _scrollBarThumb.enableFlag(ml::Flag::DRAGGABLE);
	    _scrollBarThumb.setState(ml::Draggable::State::LOCK_X);

	    enableFlag(Flag::VERTICAL);
	    enableFlag(Flag::DIRTY);

	    // ── Mouse wheel scrolling ─────────────────────────────────────────────────
	    onScroll([this](const std::optional<sf::Event>& event)
	    {
	        if (!event) return;
	        if (const auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>())
	        {
	            if (checkFlag(Flag::VERTICAL))
	            {
	                float contentHeight = getTotalContentHeight();
	                if (contentHeight > _height)
	                {
	                    float maxScroll = contentHeight - _height;
	                    _scrollOffsetY -= scroll->delta * _scrollSpeed;
	                    _scrollOffsetY  = std::clamp(_scrollOffsetY, 0.f, maxScroll);
	                    enableFlag(Flag::DIRTY);
	                    updateScrollBar();
	                }
	            }
	            if (checkFlag(Flag::HORIZONTAL))
	            {
	                float maxScroll = getTotalContentWidth() - _width;
	                _scrollOffsetX -= scroll->delta * _scrollSpeed;
	                _scrollOffsetX  = std::clamp(_scrollOffsetX, 0.f, std::max(0.f, maxScroll));
	                enableFlag(Flag::DIRTY);
	            }
	        }
	    });

	    // ── Sync scroll offset when thumb is dragged ──────────────────────────────
	    // The thumb is a separate Rectangle — subscribe to its drag via onUpdate
	    // since drag events are delivered per-component by DraggableDispatcher
	    onUpdate([this]()
	    {
	        if (!_scrollBarThumb.checkFlag(ml::Draggable::Flag::DRAGGING)) return;

	        float contentHeight = getTotalContentHeight();
	        float maxScroll     = contentHeight - _height;
	        if (maxScroll <= 0.f) return;

	        float thumbHeight = _scrollBarThumb.getGlobalBounds().size.y;
	        float trackHeight = _height - thumbHeight;
	        if (trackHeight <= 0.f) return;

	        float thumbY   = _scrollBarThumb.getPosition().y - _position.y;
	        _scrollOffsetY = (thumbY / trackHeight) * maxScroll;
	        _scrollOffsetY = std::clamp(_scrollOffsetY, 0.f, maxScroll);
	        enableFlag(Flag::DIRTY);
	    });
	}

	// ── Public API ────────────────────────────────────────────────────────────────

	void ScrollPane::addComponent(ml::Core& component)
	{
	    if (_embedded)
	        component.unsubscribeAll();
	    Core::addComponent(component);
	    enableFlag(Flag::DIRTY);
	    updateScrollBar();
	}

	void ScrollPane::removeComponent(ml::Core& component)
	{
	    Core::removeComponent(component);
	    enableFlag(Flag::DIRTY);
	    updateScrollBar();
	}

	void ScrollPane::setBackgroundColor(sf::Color color) { _background.setFillColor(color); }
	void ScrollPane::setScrollBarColor(sf::Color color)  { _scrollBarThumbColor = color; _scrollBarThumb.setFillColor(color); }
	void ScrollPane::setScrollBarWidth(float width)      { _scrollBarWidth = width; updateScrollBar(); }

	// ── Positioning ───────────────────────────────────────────────────────────────

	void ScrollPane::setPosition(const sf::Vector2f& position)
	{
	    _position = position;
	    _background.setPosition(position);
	    enableFlag(Flag::DIRTY);
	    updateScrollBar();
	}

	sf::Vector2f     ScrollPane::getPosition()     const { return _position; }
	sf::FloatRect    ScrollPane::getGlobalBounds() const { return sf::FloatRect(_position, {_width, _height}); }
	sf::RenderStates ScrollPane::getRenderStates() const { return sf::RenderStates::Default; }

	// ── Draw ──────────────────────────────────────────────────────────────────────

	void ScrollPane::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
	    stackChildren();

	    target.draw(_background, states);

	    // Children are positioned in real on-screen coords by stackChildren(),
	    // so we draw them directly inside a clipped view — keeps hit-boxes
	    // aligned with visuals (clickable children stay interactive) and
	    // composes correctly under nested views.
	    if (!getChildren().empty())
	    {
	        ml::withClipView(target, {_position, {_width, _height}}, [&]{
	            for (const auto& entry : getChildren())
	            {
	                ml::Core* child = entry.component;
	                if (!child->isVisible()) continue;

	                const sf::Vector2f  pos    = child->getPosition();
	                const sf::FloatRect bounds = child->getGlobalBounds();

	                // Cull fully off-pane children — saves draw calls when many
	                // children sit off-screen. The viewport handles partial clips.
	                if (pos.y + bounds.size.y < _position.y || pos.y > _position.y + _height) continue;
	                if (pos.x + bounds.size.x < _position.x || pos.x > _position.x + _width)  continue;

	                if (auto* drawable = dynamic_cast<sf::Drawable*>(child))
	                    target.draw(*drawable, states);
	            }
	        });
	    }

	    float contentHeight = getTotalContentHeight();
	    if (checkFlag(Flag::VERTICAL) && contentHeight > _height)
	    {
	        target.draw(_scrollBarTrack, states);
	        target.draw(_scrollBarThumb, states);
	    }
	}

	// ── Private Helpers ───────────────────────────────────────────────────────────

	void ScrollPane::stackChildren() const
	{
	    if (!checkFlag(Flag::DIRTY)) return;

	    float yOffset = 0.f;
	    for (const auto& entry : getChildren())
	    {
	        ml::Core* child = entry.component;
	        child->setPosition({
	            _position.x - _scrollOffsetX,
	            _position.y + yOffset - _scrollOffsetY
	        });
	        yOffset += child->getGlobalBounds().size.y;
	    }

	    const_cast<ScrollPane*>(this)->disableFlag(Flag::DIRTY);
	}

	float ScrollPane::getTotalContentHeight() const
	{
	    if (_contentHeightOverride > 0.f) return _contentHeightOverride;
	    float total = 0.f;
	    for (const auto& entry : getChildren())
	        total += entry.component->getGlobalBounds().size.y;
	    return total;
	}

	float ScrollPane::getTotalContentWidth() const
	{
	    float total = 0.f;
	    for (const auto& entry : getChildren())
	        total += entry.component->getGlobalBounds().size.x;
	    return total;
	}

	void ScrollPane::updateScrollBar() const
	{
	    float contentHeight = getTotalContentHeight();
	    if (contentHeight <= 0.f) return;

	    float ratio       = std::min(1.f, _height / contentHeight);
	    float thumbHeight = std::max(20.f, _height * ratio);
	    float maxScroll   = contentHeight - _height;
	    float scrollRatio = (maxScroll > 0.f) ? (_scrollOffsetY / maxScroll) : 0.f;
	    float thumbY      = _position.y + scrollRatio * (_height - thumbHeight);

	    _scrollBarThumb.setSize({_scrollBarWidth, thumbHeight});
	    _scrollBarThumb.setPosition({_position.x + _width - _scrollBarWidth, thumbY});

	    _scrollBarThumb.setDragBounds(sf::FloatRect{
	        {_position.x + _width - _scrollBarWidth, _position.y},
	        {_scrollBarWidth, _height}
	    });

	    _scrollBarTrack.setSize({_scrollBarWidth, _height});
	    _scrollBarTrack.setFillColor(_scrollBarTrackColor);
	    _scrollBarTrack.setPosition({_position.x + _width - _scrollBarWidth, _position.y});
	}
	void ScrollPane::setSize(float width, float height)
	{
		_width  = width;
		_height = height;
		_background.setSize({width, height});
	}
	void ScrollPane::setScrollOffsetY(float y)
	{
	    const float maxScroll = std::max(0.f, getTotalContentHeight() - _height);
	    _scrollOffsetY = std::clamp(y, 0.f, maxScroll);
	    enableFlag(Flag::DIRTY);
	    updateScrollBar();
	}

	void ScrollPane::setContentHeight(float height)
	{
	    _contentHeightOverride = height;
	    // Clamp current offset in case content shrank below scroll position
	    const float maxScroll = std::max(0.f, getTotalContentHeight() - _height);
	    _scrollOffsetY = std::clamp(_scrollOffsetY, 0.f, maxScroll);
	    enableFlag(Flag::DIRTY);
	    updateScrollBar();
	}

	// void ScrollPane::setScrollOffsetY(float y)
	// {
	//     const float maxScroll = std::max(0.f, getTotalContentHeight() - _height);
	//     _scrollOffsetY = std::clamp(y, 0.f, maxScroll);
	//     enableFlag(Flag::DIRTY);
	//     updateScrollBar();
	// }
	//
	// void ScrollPane::setContentHeight(float height)
	// {
	//     _contentHeightOverride = height;
	//     // Clamp current offset in case content shrank below scroll position
	//     const float maxScroll = std::max(0.f, getTotalContentHeight() - _height);
	//     _scrollOffsetY = std::clamp(_scrollOffsetY, 0.f, maxScroll);
	//     enableFlag(Flag::DIRTY);
	//     updateScrollBar();
	// }

	void ScrollPane::embed()
	{
		// Silence this pane and its internal thumb from the event system.
		// Both are Components with auto-subscriptions from ComponentCore.
		_embedded = true;
		this->unsubscribeAll();
		_scrollBarThumb.unsubscribeAll();
	}
	float ScrollPane::getScrollOffsetY() const
	{
		return _scrollOffsetY;
	}
} // namespace ml
