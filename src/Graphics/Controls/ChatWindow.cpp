//
// Created by Dave Smith on 4/30/26.
//

#include <Malena/Graphics/Controls/ChatWindow.h>
#include <limits>
namespace ml
{
    ChatWindow::ChatWindow() :
        _scrollPane(_size.x - 2.f * EDGE_PAD,
                    _size.y - INPUT_H - 3.f * EDGE_PAD)
    {
        _sendBtn.setString("Send");
        _sendBtn.setCharacterSize(15);   // without this the label falls back to the oversized default
        _sendBtn.setFillColor(sf::Color(0, 122, 255));
        _sendBtn.setTextColor(sf::Color::White);
        _sendBtn.onClick([this]() { doSend(); });

        _input.setPlaceholder("Message...");
        _input.onSubmit([this](const std::string&) { doSend(); });

        // ScrollPane is a private member — silence it from the event system so it
        // cannot steal click focus away from _input.  Mouse wheel is re-routed via
        // ChatWindow's own onScroll below; scrollbar-thumb drag is not supported.
        _scrollPane.embed();

        // Forward mouse-wheel events to the scroll pane from the whole chat area.
        onScroll([this](const std::optional<sf::Event>& event)
        {
            if (!event) return;
            const auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>();
            if (!scroll) return;
            float newY = _scrollPane.getScrollOffsetY() - scroll->delta * 20.f;
            _scrollPane.setScrollOffsetY(newY);
        });

        // Register members so Core's enable/disable cascade reaches them.
        addComponents(_scrollPane, _input, _sendBtn);
    }

    void ChatWindow::applyLayout()
    {
        _scrollPane.setPosition({_position.x + EDGE_PAD, _position.y + EDGE_PAD});

        float inputY = _position.y + _size.y - INPUT_H - EDGE_PAD;
        float inputW = _size.x - 2.f * EDGE_PAD - BTN_W - EDGE_PAD;

        _input.setSize({inputW, INPUT_H});
        _input.setPosition({_position.x + EDGE_PAD, inputY});

        _sendBtn.setSize({BTN_W, INPUT_H});
        _sendBtn.setPosition({_position.x + _size.x - BTN_W - EDGE_PAD, inputY});
    }

    void ChatWindow::addMessage(const ChatMessage& msg)
    {
        _messages.push_back(msg);

        float containerW = _size.x - 2.f * EDGE_PAD;
        auto  bubble     = std::make_unique<ChatBubble>();
        bubble->setMessage(msg, containerW);
        _scrollPane.addComponent(*bubble);
        _bubbles.push_back(std::move(bubble));

        _scrollPane.setScrollOffsetY(std::numeric_limits<float>::max());
    }

    void ChatWindow::clear()
    {
        for (auto& b : _bubbles)
            _scrollPane.removeComponent(*b);
        _bubbles.clear();
        _messages.clear();
    }

    void ChatWindow::onSend(std::function<void(const std::string&)> callback)
    {
        _onSend = std::move(callback);
    }

    void ChatWindow::setSize(const sf::Vector2f& size)
    {
        _size = size;
        _scrollPane.setSize(size.x - 2.f * EDGE_PAD, size.y - INPUT_H - 3.f * EDGE_PAD);
        applyLayout();
        rebuildBubbles();
    }

    void ChatWindow::rebuildBubbles()
    {
        for (auto& b : _bubbles)
            _scrollPane.removeComponent(*b);
        _bubbles.clear();

        float containerW = _size.x - 2.f * EDGE_PAD;
        for (const auto& msg : _messages) {
            auto bubble = std::make_unique<ChatBubble>();
            bubble->setMessage(msg, containerW);
            _scrollPane.addComponent(*bubble);
            _bubbles.push_back(std::move(bubble));
        }

        _scrollPane.setScrollOffsetY(std::numeric_limits<float>::max());
    }

    void ChatWindow::doSend()
    {
        auto text = _input.getValue();
        if (text.empty()) return;
        _input.setValue("");
        if (_onSend) _onSend(text);
    }

    void ChatWindow::setPosition(const sf::Vector2f& pos)
    {
        _position = pos;
        applyLayout();
    }

    sf::Vector2f ChatWindow::getPosition() const
    {
        return _position;
    }

    sf::FloatRect ChatWindow::getGlobalBounds() const
    {
        return {_position, _size};
    }

}
