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
        _sendBtn.setFillColor(sf::Color(0, 122, 255));
        _sendBtn.setTextColor(sf::Color::White);
        _sendBtn.onClick([this]() { doSend(); });

        _input.setPlaceholder("Message...");
        _input.onSubmit([this](const std::string&) { doSend(); });
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

    void ChatWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(_scrollPane, states);
        target.draw(_input, states);
        target.draw(_sendBtn, states);
    }
}