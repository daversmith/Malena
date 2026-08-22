//
// Created by Dave Smith on 4/30/26.
//

#include <Malena/Graphics/Controls/ChatBubble.h>
#include <Malena/Utilities/Utf8.h>
#include <algorithm>
#include <limits>
namespace ml
{
    ChatBubble::ChatBubble()
    {
        _bg.setRadius(CORNER_R);
        _senderText.setCharacterSize(12);
        _messageText.setCharacterSize(15);
        _messageText.setWordWrap(true);
        _timestampText.setCharacterSize(11);
    }

    void ChatBubble::setMessage(const ChatMessage& msg, float containerWidth)
    {
        _isMine         = msg.isMine;
        _containerWidth = containerWidth;

        // ── Sender label (only shown for others' messages) ────────────────────────
        if (!msg.isMine && !msg.sender.empty()) {
            _senderText.setString(ml::utf8(msg.sender));
            _senderText.setFillColor(sf::Color(160, 160, 255));
            _senderH = _senderText.getGlobalBounds().size.y + SENDER_GAP;
        } else {
            _senderH = 0.f;
        }

        // ── Message text ─────────────────────────────────────────────────────────
        float textMaxW = containerWidth * MAX_W_RATIO - 2.f * PADDING;
        _messageText.setMaxWidth(textMaxW);
        _messageText.setString(ml::utf8(msg.text));
        _messageText.setFillColor(sf::Color::White);

        float msgW = std::min(_messageText.getGlobalBounds().size.x, textMaxW);
        float msgH = _messageText.getGlobalBounds().size.y;

        // ── Timestamp ─────────────────────────────────────────────────────────────
        _timestampText.setString(ml::utf8(msg.timestamp));
        _timestampText.setFillColor(sf::Color(255, 255, 255, 160));

        float tsW = _timestampText.getGlobalBounds().size.x;
        float tsH = _timestampText.getGlobalBounds().size.y;

        // ── Bubble geometry ───────────────────────────────────────────────────────
        float bubbleW = std::max(msgW, tsW) + 2.f * PADDING;
        float bubbleH = PADDING + msgH + TS_GAP + tsH + PADDING;
        _bubbleSize   = {bubbleW, bubbleH};

        _totalHeight = _senderH + bubbleH + 6.f; // 6 px gap below each bubble

        // ── Background color ──────────────────────────────────────────────────────
        _bg.setFillColor(msg.isMine ? sf::Color(0, 122, 255)
                                    : sf::Color(58, 58, 60));
        _bg.setSize(_bubbleSize);

        applyLayout();
    }

    void ChatBubble::applyLayout()
    {
        if (_containerWidth <= 0.f) return;

        // Bubble x — right-aligned for mine, left-aligned for others
        float bgX = _isMine ? _position.x + _containerWidth - _bubbleSize.x - EDGE_MARGIN
                            : _position.x + EDGE_MARGIN;
        float bgY = _position.y + _senderH;

        _bg.setPosition({bgX, bgY});
        _messageText.setPosition({bgX + PADDING, bgY + PADDING});

        // Timestamp — bottom-right inside bubble for mine, bottom-left for others
        float tsY = bgY + _bubbleSize.y - _timestampText.getGlobalBounds().size.y - PADDING * 0.5f;
        float tsX = _isMine ? bgX + _bubbleSize.x - _timestampText.getGlobalBounds().size.x - PADDING * 0.5f
                            : bgX + PADDING;
        _timestampText.setPosition({tsX, tsY});

        // Sender label sits above the bubble on the same side
        if (!_isMine)
            _senderText.setPosition({bgX, _position.y});
    }

    void ChatBubble::setPosition(const sf::Vector2f& pos)
    {
        _position = pos;
        applyLayout();
    }

    sf::Vector2f ChatBubble::getPosition() const
    {
        return _position;
    }

    sf::FloatRect ChatBubble::getGlobalBounds() const
    {
        return {_position, {_containerWidth, _totalHeight}};
    }

    void ChatBubble::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(_bg, states);
        if (!_isMine && _senderH > 0.f)
            target.draw(_senderText, states);
        target.draw(_messageText, states);
        target.draw(_timestampText, states);
    }
}