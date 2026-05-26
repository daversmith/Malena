//
// Created by Dave Smith on 4/30/26.
//

#ifndef CHATWINDOW_H
#define CHATWINDOW_H


#include <Malena/Graphics/Controls/ChatBubble.h>
#include <Malena/Graphics/Controls/ScrollPane.h>
#include <Malena/Graphics/Text/TextInput.h>
#include <Malena/Graphics/Controls/RectangleButton.h>
#include <functional>
#include <vector>
#include <memory>
#include <string>

#include "Malena/Traits/Communication/Messenger.h"
// A reusable text-messaging-style chat window.
//
// Data flows in via addMessage() — the window has no knowledge of WebSockets or
// any transport layer. The caller is responsible for delivering messages and for
// handling outbound messages via onSend().
//
// Call setSize() to resize (e.g. panel.addComponent(chat, true) fill support).
namespace ml
{
    class ChatWindow : public ml::Component<Messenger>
    {
        static constexpr float INPUT_H  = 54.f;
        static constexpr float BTN_W    = 72.f;
        static constexpr float EDGE_PAD = 8.f;

        // _size must be declared before _scrollPane so its in-class initializer
        // runs first and can be used to compute the scroll pane's initial dimensions.
        sf::Vector2f _size{400.f, 500.f};

        ml::ScrollPane      _scrollPane;
        ml::TextInput       _input;
        ml::RectangleButton _sendBtn;


        std::vector<ChatMessage>                 _messages;
        std::vector<std::unique_ptr<ChatBubble>> _bubbles;
        sf::Vector2f _position;
        std::function<void(const std::string&)>  _onSend;

        void applyLayout();
        void doSend();
        void rebuildBubbles();

    public:
        ChatWindow();

        // Append a message bubble. Called by the app layer when a message arrives
        // (from WebSocket, local input echo, etc.).
        void addMessage(const ChatMessage& msg);
        void clear();

        // Register a callback fired when the user sends a message.
        // The callback receives the raw text; the caller is responsible for
        // transmitting it and echoing it back via addMessage() if desired.
        void onSend(std::function<void(const std::string&)> callback);

        // Resize the window — enables use with Panel::addComponent(chat, true).
        void setSize(const sf::Vector2f& size);

        void          setPosition(const sf::Vector2f& pos) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
        void setRecipientId(const std::string& id);
        const std::string& getRecipientId() const;

        void setEnabled(bool enabled)       override;
        void setParentEnabled(bool enabled) override;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    };
}


#endif //CHATWINDOW_H
