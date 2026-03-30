//
// TextArea.h
//

#ifndef TEXTAREA_H
#define TEXTAREA_H

#pragma once

#include <Malena/Graphics/Text/TextInput.h>
#include <Malena/Graphics/Controls/ScrollPane.h>

namespace ml
{
    /**
     * @brief A multi-line rich text input field with scrolling.
     * @ingroup GraphicsText
     *
     * @c TextArea extends the same rich text model as @c TextInput to
     * multiple lines. It uses @c ScrollPane internally for overflow scrolling
     * and supports all the same keyboard shortcuts plus line navigation.
     *
     * @c Enter inserts a newline. @c Ctrl+Enter submits.
     * @c Up / @c Down arrows move between lines preserving horizontal position.
     *
     * ### Additional keyboard shortcuts (beyond TextInput)
     * | Key          | Action                          |
     * |--------------|---------------------------------|
     * | Enter        | Insert newline                  |
     * | Ctrl+Enter   | Submit                          |
     * | Up / Down    | Move cursor between lines       |
     * | Shift+Up/Down| Extend selection between lines  |
     *
     * ### Usage
     * @code
     * ml::TextArea bio;
     * bio.setSize({400.f, 120.f});
     * bio.setPosition({100.f, 200.f});
     * bio.setPlaceholder("Enter your bio...");
     *
     * bio.onChange([](const std::string& value){
     *     std::cout << value << "\n";
     * });
     * bio.onSubmit([](const std::string& value){
     *     std::cout << "submitted\n";
     * });
     *
     * addComponent(bio);
     * @endcode
     *
     * ### Rich text styling
     * @code
     * bio.selectAll();
     * bio.setSelectionColor(sf::Color::Cyan);
     * bio.setSelectionBold(true);
     * @endcode
     *
     * ### With a manifest
     * @code
     * class NotesField : public ml::TextAreaWith<NotesManifest> {};
     * @endcode
     *
     * @see TextInput, TextInputManifest, RichTextBuffer, RichTextRenderer, ScrollPane
     */
    class TextArea : public TextInput
    {
    private:
        ScrollPane _scrollPane;

        void handleTextAreaKeypress(const sf::Event::KeyPressed& kp);
        void updateScroll();

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    public:
        /**
         * @brief Construct a multi-line rich text area.
         * @param font Default font. Defaults to built-in Arial.
         */
        explicit TextArea(const sf::Font& font = FontManager<>::getDefault());

        // ── Scrollbar styling ─────────────────────────────────────────────────

        /** @brief Set the scrollbar thumb color. */
        void setScrollBarColor(const sf::Color& color);

        /** @brief Set the scrollbar track color. */
        void setScrollBarTrackColor(const sf::Color& color);

        /** @brief Set the scrollbar width in pixels. */
        void setScrollBarWidth(float width);

        // ── TextInput overrides ───────────────────────────────────────────────

        void setSize(const sf::Vector2f& size);
        void setPosition(const sf::Vector2f& position) override;
        sf::FloatRect getGlobalBounds() const override;

	protected:
		void reflow() override;
		void syncPlaceholder() override;
	};

    // ── TextAreaWith ──────────────────────────────────────────────────────────

    /**
     * @brief @c TextArea with an attached manifest.
     * @tparam MANIFEST A @c Manifest subclass declaring @c Flag/@c State enums.
     * @see TextArea, Customizable
     */
    template<typename MANIFEST>
    class TextAreaWith : public TextArea, public Customizable<MANIFEST>
    {
    public:
        using TextArea::TextArea;
    };

} // namespace ml

#endif // TEXTAREA_H
