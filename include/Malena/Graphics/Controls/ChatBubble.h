//
// Created by Dave Smith on 4/30/26.
//

#ifndef CHATBUBBLE_H
#define CHATBUBBLE_H

#include <Malena/Engine/Messaging/ChatMessage.h>
#include <Malena/Core/Component.h>
#include <Malena/Traits/Communication/Messenger.h>
#include <Malena/Graphics/Primitives/RoundedRectangle.h>
#include <Malena/Graphics/Text/Text.h>

// A single message bubble — left-aligned for others, right-aligned for mine.
// Add to a ScrollPane (or any ml::Core container) to include it in layout.
// Call setMessage() whenever the message content or container width changes.
namespace ml
{
    class ChatBubble : public ml::Component<ml::Messenger>
    {
        static constexpr float PADDING     = 12.f;
        static constexpr float CORNER_R    = 14.f;
        static constexpr float MAX_W_RATIO = 0.72f;
        static constexpr float EDGE_MARGIN = 8.f;
        static constexpr float TS_GAP      = 4.f;
        static constexpr float SENDER_GAP  = 3.f;

        ml::RoundedRectangle _bg;
        ml::Text             _senderText;
        ml::Text             _messageText;
        ml::Text             _timestampText;

        bool         _isMine         = false;
        float        _containerWidth = 0.f;
        float        _totalHeight    = 0.f;
        float        _senderH        = 0.f;
        sf::Vector2f _bubbleSize;
        sf::Vector2f _position;

        void applyLayout();

    public:
        ChatBubble();

        // Feed message data and the pixel width of the containing scroll area.
        // Re-call when the container is resized.
        void setMessage(const ChatMessage& msg, float containerWidth);

        float getHeight() const { return _totalHeight; }

        void          setPosition(const sf::Vector2f& pos) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    };
}


#endif //CHATBUBBLE_H
