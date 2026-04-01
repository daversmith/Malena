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
#include <Malena/Traits/Settings/TextInputSettings.h>
#include <Malena/Traits/Theme/TextInputTheme.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <type_traits>

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
     *
     * Inherits @c TextInputSettings (geometry/behaviour) and
     * @c TextInputTheme (colors/font) as first-class members.
     *
     * @see TextInputSettings, TextInputTheme, TextInputStyle, TextArea
     */
    class TextInput : public ComponentWith<TextInputManifest>,
                      public TextInputSettings,
                      public TextInputTheme,
                      public Themeable
    {
    public:
        using Flag  = TextInputManifest::Flag;
        using State = TextInputManifest::State;

    protected:
        RichTextBuffer            _buffer;
        RichTextRenderer          _renderer;
        mutable sf::RenderTexture _canvas;

        sf::RectangleShape _background;
        sf::Vector2f       _position = {0.f, 0.f};
        float              _scrollX  = 0.f;

        sf::Text _placeholder;
        bool     _showPlaceholder = true;

        mutable sf::Clock _cursorClock;
        mutable bool      _cursorVisible = false;

        bool         _dragging      = false;
        bool         _prevMouseDown = false;
        std::size_t  _dragAnchor    = 0;
        sf::Clock    _clickClock;
        sf::Vector2f _lastClick     = {};
        bool         _waitingDouble = false;

        std::function<void(const std::string&)> _onChange;
        std::function<void(const std::string&)> _onSubmit;

        void rebuild();
        virtual void onRebuildComplete() {}
        virtual void reflow();
        virtual void rebuildAndScroll();
        void syncColors();
        virtual void syncPlaceholder();
        virtual std::size_t hitTest(const sf::Vector2f& worldPos) const;

        virtual void handleKey(const sf::Event::KeyPressed& kp);
        void         handleChar(const sf::Event::TextEntered& te);

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit TextInput(const sf::Font& font = FontManager<>::getDefault());

        // ── Apply ─────────────────────────────────────────────────────────────

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<TextInputSettings, S>,
                "applySettings() requires a type derived from TextInputSettings");
            static_cast<TextInputSettings&>(*this) = s;
            syncColors();
            syncPlaceholder();
            rebuild();
        }

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<TextInputTheme, T>,
                "applyTheme() requires a type derived from TextInputTheme");
            static_cast<TextInputTheme&>(*this) = t;
            syncColors();
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<TextInputSettings, St> &&
                          std::is_base_of_v<TextInputTheme, St>,
                "applyStyle() requires TextInputSettings and TextInputTheme");
            static_cast<TextInputSettings&>(*this) = s;
            static_cast<TextInputTheme&>(*this)    = s;
            syncColors();
            syncPlaceholder();
            rebuild();
        }

        // ── Value ─────────────────────────────────────────────────────────────

        void setValue(const std::string& value);
        [[nodiscard]] std::string getValue() const;
        void clear();

        // ── Selection styling ─────────────────────────────────────────────────

        void setSelectionFont(const sf::Font& font);
        void setSelectionCharSize(unsigned int size);
        void setSelectionColor(const sf::Color& color);
        void setSelectionBold(bool bold);
        void setSelectionItalic(bool italic);
        void setSelectionUnderline(bool underline);

        void selectAll();
        void setSelection(std::size_t start, std::size_t end);
        [[nodiscard]] std::string getSelectedText() const;

        // ── Options ───────────────────────────────────────────────────────────

        void setPlaceholder(const std::string& text);
        [[nodiscard]] std::string getPlaceholder() const;

        void setEnabled(bool enabled);
        [[nodiscard]] bool isEnabled() const;
        void setReadOnly(bool readonly);
        [[nodiscard]] bool isReadOnly() const;

        void setError(bool error);
        [[nodiscard]] bool hasError() const;

        // ── Size / font ───────────────────────────────────────────────────────

        virtual void setSize(const sf::Vector2f& size);
        [[nodiscard]] sf::Vector2f getSize() const;

        virtual void setFont(const sf::Font& font);
        void setFont(const sf::Font&&) = delete;
        virtual void setCharacterSize(unsigned int size);
        [[nodiscard]] unsigned int getCharacterSize() const;

        // ── Callbacks ─────────────────────────────────────────────────────────

        void onChange(std::function<void(const std::string&)> callback);
        void onSubmit(std::function<void(const std::string&)> callback);

        // ── Positionable ──────────────────────────────────────────────────────

        virtual void  setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class TextInputWith : public TextInput, public Customizable<MANIFEST>
    { public: using TextInput::TextInput; };

} // namespace ml

#endif // TEXTINPUT_H
