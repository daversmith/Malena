//
// TextInput.h
//

#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#pragma once

#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Graphics/Text/RichTextBuffer.h>
#include <Malena/Graphics/Text/RichTextRenderer.h>
#include <Malena/Resources/FontManager.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

namespace ml
{
    class TextInputManifest : public ml::Manifest
    {
    public:
        enum class Flag  { DISABLED, READONLY };
        enum class State { IDLE, FOCUSED, DISABLED, ERROR };
    };

    /**
     * @brief Single-line rich text input with horizontal scrolling.
     * @ingroup GraphicsText
     */
    class TextInput : public ComponentWith<TextInputManifest>
    {
    public:
        using Flag  = TextInputManifest::Flag;
        using State = TextInputManifest::State;

    protected:
        RichTextBuffer            _buffer;
        RichTextRenderer          _renderer;
        mutable sf::RenderTexture _canvas;

        sf::RectangleShape _background;
        sf::Vector2f       _size     = {200.f, 36.f};
        sf::Vector2f       _position = {0.f, 0.f};
        float              _padding  = 8.f;
        float              _scrollX  = 0.f;

        sf::Color _bgIdle      = sf::Color(40,  40,  40);
        sf::Color _bgFocused   = sf::Color(50,  50,  50);
        sf::Color _bgDisabled  = sf::Color(30,  30,  30);
        sf::Color _borderIdle  = sf::Color(100, 100, 100);
        sf::Color _borderFocus = sf::Color(70,  130, 230);
        sf::Color _borderError = sf::Color(220, 70,  70);
        sf::Color _borderDis   = sf::Color(60,  60,  60);
        sf::Color _textColor   = sf::Color::White;
        sf::Color _phColor     = sf::Color(120, 120, 120);
        sf::Color _selColor    = sf::Color(70,  130, 230, 120);
        sf::Color _cursorColor = sf::Color::White;
        float     _borderThick = 1.5f;

        sf::Text _placeholder;
        bool     _showPlaceholder = true;

        const sf::Font* _font     = &FontManager<>::getDefault();
        unsigned int    _charSize = 16;

        // Cursor blink
        mutable sf::Clock _cursorClock;
        mutable bool      _cursorVisible = false;

        // Options
        std::size_t _maxLength    = 0;
        bool        _passwordMode = false;
        char32_t    _passwordChar = U'\u2022';

        // Drag + double-click — driven by polling in onUpdate
        bool         _dragging      = false;
        bool         _prevMouseDown = false;
        std::size_t  _dragAnchor    = 0;
        sf::Clock    _clickClock;
        sf::Vector2f _lastClick     = {};
        bool         _waitingDouble = false;

        std::function<void(const std::string&)> _onChange;
        std::function<void(const std::string&)> _onSubmit;

        void rebuild();         ///< Full rebuild — recreates sf::Text objects
        virtual void reflow();          ///< Reposition only — never creates sf::Text objects
        void rebuildAndScroll();
        void syncColors();
        virtual void syncPlaceholder();
        std::size_t hitTest(const sf::Vector2f& worldPos) const;

        virtual void handleKey(const sf::Event::KeyPressed& kp);
        void         handleChar(const sf::Event::TextEntered& te);

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    public:
        explicit TextInput(const sf::Font& font = FontManager<>::getDefault());

        void setValue(const std::string& value);
        [[nodiscard]] std::string getValue() const;
        void clear();

        void setSelectionFont(const sf::Font& font);
        void setSelectionCharSize(unsigned int size);
        void setSelectionColor(const sf::Color& color);
        void setSelectionBold(bool bold);
        void setSelectionItalic(bool italic);
        void setSelectionUnderline(bool underline);

        void selectAll();
        void setSelection(std::size_t start, std::size_t end);
        [[nodiscard]] std::string getSelectedText() const;

        void setMaxLength(std::size_t length);
        [[nodiscard]] std::size_t getMaxLength() const;
        void setPasswordMode(bool enabled);
        [[nodiscard]] bool isPasswordMode() const;
        void setPasswordChar(char32_t ch);

        void setPlaceholder(const std::string& text);
        [[nodiscard]] std::string getPlaceholder() const;

        void setEnabled(bool enabled);
        [[nodiscard]] bool isEnabled() const;
        void setReadOnly(bool readonly);
        [[nodiscard]] bool isReadOnly() const;

        void setError(bool error);
        [[nodiscard]] bool hasError() const;

        virtual void setSize(const sf::Vector2f& size);
        [[nodiscard]] sf::Vector2f getSize() const;
        void setBackgroundColor(const sf::Color& color);
        void setBackgroundFocusedColor(const sf::Color& color);
        void setBorderColor(const sf::Color& color);
        void setBorderFocusedColor(const sf::Color& color);
        void setBorderErrorColor(const sf::Color& color);
        void setBorderThickness(float thickness);
        void setDefaultTextColor(const sf::Color& color);
        void setPlaceholderColor(const sf::Color& color);
        void setSelectionHighlightColor(const sf::Color& color);
        void setCursorColor(const sf::Color& color);
        void setPadding(float padding);
        [[nodiscard]] float getPadding() const;

        virtual void setFont(const sf::Font& font);
        void setFont(const sf::Font&&) = delete;
        virtual void setCharacterSize(unsigned int size);
        [[nodiscard]] unsigned int getCharacterSize() const;

        void onChange(std::function<void(const std::string&)> callback);
        void onSubmit(std::function<void(const std::string&)> callback);

        virtual void  setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class TextInputWith : public TextInput, public Customizable<MANIFEST>
    {
    public:
        using TextInput::TextInput;
    };

} // namespace ml

#endif // TEXTINPUT_H
