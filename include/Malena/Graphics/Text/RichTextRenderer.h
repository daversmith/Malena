//
// RichTextRenderer.h
//

#ifndef RICHTEXTRENDERER_H
#define RICHTEXTRENDERER_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Graphics/Text/RichTextBuffer.h>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <vector>

namespace ml
{
    // ── RenderedSegment ───────────────────────────────────────────────────────

    /**
     * @brief A single styled run of text ready for rendering.
     *
     * Owns an @c sf::Text constructed once during @c rebuild() and
     * repositioned in place during @c reflow(). The @c sf::Text is never
     * recreated after initial construction — only its position changes.
     *
     * @see RichTextRenderer
     */
    struct RenderedSegment
    {
        sf::Text     sfText;       ///< SFML text object, positioned and styled
        std::size_t  bufStart = 0; ///< First char index in the buffer (inclusive)
        std::size_t  bufEnd   = 0; ///< Last char index in the buffer (exclusive)
        sf::Vector2f position;     ///< Top-left position

        RenderedSegment(sf::Text&&   text,
                        std::size_t  start,
                        std::size_t  end,
                        sf::Vector2f pos)
            : sfText(std::move(text)),
              bufStart(start),
              bufEnd(end),
              position(pos)
        {}
    };

    // ── RenderedLine ──────────────────────────────────────────────────────────

    /**
     * @brief One visual line of text made up of @c RenderedSegment objects.
     */
    struct RenderedLine
    {
        std::vector<RenderedSegment> segments;
        float       y         = 0.f;
        float       height    = 0.f;  ///< Total height including word-wrap rows
        float       rowHeight = 0.f;  ///< Single visual row height (for cursor/selection)
        std::size_t charStart = 0;
        std::size_t charEnd   = 0;
    };

    // ── RichTextRenderer ─────────────────────────────────────────────────────

    /**
     * @brief Rendering layer for @c RichTextBuffer.
     *
     * Converts a @c RichTextBuffer into positioned @c sf::Text segments.
     *
     * ### Two-phase design
     * - @c rebuild() — recreates @c sf::Text objects from buffer content.
     *   Call when text or styling changes.
     * - @c setOrigin() / @c reflow() — repositions existing @c sf::Text objects
     *   in place. Call when only scroll offset or field position changes.
     *   Never creates new @c sf::Text objects — safe to call every frame.
     *
     * @see RichTextBuffer, TextInput, TextArea
     */
    class MALENA_API RichTextRenderer
    {
    public:
        RichTextRenderer(const RichTextBuffer& buffer,
                         const sf::Font&       defaultFont,
                         unsigned int          defaultSize,
                         sf::Color             defaultColor);

        // ── Layout control ────────────────────────────────────────────────────

        /**
         * @brief Set the top-left origin and reposition segments.
         *
         * Calls @c reflow() — never recreates @c sf::Text objects.
         * Safe to call every frame for scroll/position changes.
         */
        void setOrigin(const sf::Vector2f& origin);

        /** @brief Set max line width for wrapping. @c 0 = no wrap. */
        void setMaxWidth(float width);

        /**
         * @brief Rebuild @c sf::Text objects from the current buffer content.
         *
         * Call when text or styling actually changes.
         */
        void rebuild();

        /**
         * @brief Reposition existing @c sf::Text objects to the current origin.
         *
         * Never creates new objects — safe to call every frame.
         */
        void reflow();

        // ── Drawing ───────────────────────────────────────────────────────────

        void draw(sf::RenderTarget& target, const sf::RenderStates& states) const;

        void drawSelection(sf::RenderTarget&       target,
                           const sf::RenderStates& states,
                           std::size_t             selStart,
                           std::size_t             selEnd,
                           const sf::Color&        color) const;

        void drawCursor(sf::RenderTarget&       target,
                        const sf::RenderStates& states,
                        std::size_t             charIndex,
                        const sf::Color&        color,
                        float                   width = 2.f) const;

        // ── Coordinate mapping ────────────────────────────────────────────────

        [[nodiscard]] sf::Vector2f charIndexToPosition(std::size_t index) const;
        [[nodiscard]] std::size_t  positionToCharIndex(const sf::Vector2f& pos) const;
        [[nodiscard]] std::size_t  lineIndexForChar(std::size_t charIndex) const;
        [[nodiscard]] std::size_t  charIndexAbove(std::size_t cursorIndex) const;
        [[nodiscard]] std::size_t  charIndexBelow(std::size_t cursorIndex) const;

        // ── Layout info ───────────────────────────────────────────────────────

        [[nodiscard]] float getTotalHeight() const;
        [[nodiscard]] const std::vector<RenderedLine>& getLines() const;

    private:
        const RichTextBuffer& _buffer;
        const sf::Font*       _defaultFont;
        unsigned int          _defaultSize;
        sf::Color             _defaultColor;

        sf::Vector2f              _origin;
        float                     _maxWidth = 0.f;
        std::vector<RenderedLine> _lines;

        void     buildSegments();
        void     layoutLines();
        sf::Text makeText(const std::string& str, const TextAttribute& attr) const;
    };

} // namespace ml

#endif // RICHTEXTRENDERER_H