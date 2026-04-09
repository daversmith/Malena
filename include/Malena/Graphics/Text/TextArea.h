//
// TextArea.h
//

#ifndef TEXTAREA_H
#define TEXTAREA_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Graphics/Text/TextInput.h>
#include <Malena/Graphics/Controls/ScrollPane.h>
#include <Malena/Traits/Theme/TextAreaTheme.h>
#include <type_traits>

namespace ml
{
    /**
     * @brief A multi-line rich text input field with vertical scrolling.
     * @ingroup GraphicsText
     *
     * Extends @c TextInput with multiline support. The canvas grows to fit
     * the full content height — @c ScrollPane clips and scrolls it, and
     * draws the scrollbar. @c ScrollPane is fully event-silent (@c unsubscribeAll
     * is called internally) so it never interferes with TextInput's focus or
     * keyboard handling.
     *
     * @c Enter inserts a newline. @c Ctrl+Enter submits.
     * @c Up / @c Down arrows move between lines.
     * Mouse wheel scrolls the content.
     *
     * @see TextInput, TextAreaTheme, TextAreaStyle, ScrollPane
     */
    class MALENA_API TextArea : public TextInput,
                     public TextAreaTheme
    {
    private:
        ScrollPane _scrollPane;

        void handleTextAreaKeypress(const sf::Event::KeyPressed& kp);
        void drawScrollbar(sf::RenderTarget& target, const sf::RenderStates& states) const;
        void scrollToCursor();
        void rebuildAndScroll() override;
        std::size_t hitTest(const sf::Vector2f& worldPos) const override;

        // ── Scroll / drag state ───────────────────────────────────────────
        float _prevScrollY          = 0.f;
        bool  _thumbDragging        = false;
        float _thumbDragStartMouseY = 0.f;
        float _thumbDragStartScroll = 0.f;
        bool  _prevMouseDown        = false;

    protected:
        // ── TextInput hooks ───────────────────────────────────────────────────

        /**
         * @brief Called after every rebuild(). Grows the canvas to full content
         *        height and updates ScrollPane's content height.
         */
        void onRebuildComplete() override;

        /**
         * @brief Top-aligned reflow. Reads vertical scroll from ScrollPane.
         */
        void reflow() override;

        void syncPlaceholder() override;

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit TextArea(const sf::Font& font = FontManager<>::getDefault());

        // ── Apply ─────────────────────────────────────────────────────────────

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<TextAreaTheme, T>,
                "applyTheme() requires a type derived from TextAreaTheme");
            static_cast<TextAreaTheme&>(*this) = t;
            _scrollPane.setScrollBarColor(scrollBarColor);
            _scrollPane.setBackgroundColor(scrollBarTrackColor);
            _scrollPane.setScrollBarWidth(scrollBarWidth);
            syncColors();
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<TextInputSettings, St> &&
                          std::is_base_of_v<TextAreaTheme, St>,
                "applyStyle() requires TextInputSettings and TextAreaTheme");
            static_cast<TextInputSettings&>(*this) = s;
            static_cast<TextAreaTheme&>(*this)     = s;
            _scrollPane.setScrollBarColor(scrollBarColor);
            _scrollPane.setBackgroundColor(scrollBarTrackColor);
            _scrollPane.setScrollBarWidth(scrollBarWidth);
            syncColors();
            syncPlaceholder();
            rebuild();
        }

        // ── Scrollbar styling ─────────────────────────────────────────────────

        void setScrollBarColor(const sf::Color& color);
        void setScrollBarTrackColor(const sf::Color& color);
        void setScrollBarWidth(float width);

        // ── Overrides ─────────────────────────────────────────────────────────

        void setSize(const sf::Vector2f& size)         override;
        void setPosition(const sf::Vector2f& position) override;
        sf::FloatRect getGlobalBounds()          const override;
    };

    template<typename MANIFEST>
    class TextAreaWith : public TextArea, public Customizable<MANIFEST>
    {
    public:
        using TextArea::TextArea;
    };

} // namespace ml

#endif // TEXTAREA_H
