//
// RichTextBuffer.h
//

#ifndef RICHTEXTBUFFER_H
#define RICHTEXTBUFFER_H

#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>
#include <optional>
#include <string>
#include <vector>

namespace ml
{
    // ── TextAttribute ─────────────────────────────────────────────────────────

    /**
     * @brief A styled attribute range applied to a span of characters.
     *
     * Each field is optional — @c std::nullopt means "inherit the default
     * from the parent component." Only fields that are explicitly set
     * override the component default.
     *
     * @see RichTextBuffer
     */
    struct TextAttribute
    {
        std::size_t start = 0;   ///< First character index (inclusive)
        std::size_t end   = 0;   ///< Last character index (exclusive)

        const sf::Font*               font;      ///< nullptr = inherit default font
        std::optional<unsigned int>   charSize;  ///< nullopt = inherit default size
        std::optional<sf::Color>      color;     ///< nullopt = inherit default color
        std::optional<bool>           bold;      ///< nullopt = inherit
        std::optional<bool>           italic;    ///< nullopt = inherit
        std::optional<bool>           underline; ///< nullopt = inherit

        TextAttribute() : font(nullptr) {}
    };

    // ── RichTextBuffer ────────────────────────────────────────────────────────

    /**
     * @brief Pure data layer for rich text editing.
     *
     * @c RichTextBuffer owns a flat UTF-8 string and a vector of
     * @c TextAttribute ranges that style spans of it. It manages cursor
     * position, selection, and all text editing operations. It has no
     * rendering dependencies — all SFML drawing is handled by @c RichTextRenderer.
     *
     * ### Editing model
     * All insertions happen at the cursor position. All deletions operate
     * relative to the cursor. Selection is defined by @c _selStart and
     * @c _selEnd — when they differ, a selection is active.
     *
     * ### Attribute model
     * Attributes are stored as non-overlapping ranges sorted by @c start.
     * Applying an attribute to the current selection splits existing ranges
     * at the selection boundaries and inserts or merges the new attribute.
     *
     * @see RichTextRenderer, TextInput, TextArea
     */
    class RichTextBuffer
    {
    public:
        // ── Construction ──────────────────────────────────────────────────────

        RichTextBuffer() = default;

        // ── Buffer access ─────────────────────────────────────────────────────

        /** @brief Return the full text content as a UTF-8 string. */
        [[nodiscard]] const std::string& getText() const;

        /** @brief Return the number of characters in the buffer. */
        [[nodiscard]] std::size_t size() const;

        /** @brief Return @c true if the buffer contains no characters. */
        [[nodiscard]] bool empty() const;

        /** @brief Return all attribute ranges. */
        [[nodiscard]] const std::vector<TextAttribute>& getAttributes() const;

        // ── Cursor ────────────────────────────────────────────────────────────

        /** @brief Return the current cursor position (char index). */
        [[nodiscard]] std::size_t getCursor() const;

        /**
         * @brief Set the cursor position clamped to [0, size()].
         * Clears the selection.
         *
         * @param pos New cursor position.
         */
        void setCursor(std::size_t pos);

        /**
         * @brief Move the cursor by @p delta characters, clamped to buffer bounds.
         *
         * @param delta Positive = forward, negative = backward.
         * @param extendSelection If @c true, extends the selection instead of moving it.
         */
        void moveCursor(int delta, bool extendSelection = false);

        /**
         * @brief Move the cursor to the start of the previous or next word.
         *
         * @param direction @c -1 for previous word, @c +1 for next word.
         * @param extendSelection If @c true, extends the selection.
         */
        void moveCursorByWord(int direction, bool extendSelection = false);

        /**
         * @brief Move the cursor to the start of the current line.
         * @param extendSelection If @c true, extends the selection.
         */
        void moveCursorToLineStart(bool extendSelection = false);

        /**
         * @brief Move the cursor to the end of the current line.
         * @param extendSelection If @c true, extends the selection.
         */
        void moveCursorToLineEnd(bool extendSelection = false);

        // ── Selection ─────────────────────────────────────────────────────────

        /** @brief Return @c true if a selection is active. */
        [[nodiscard]] bool hasSelection() const;

        /** @brief Return the selection start (always <= selectionEnd). */
        [[nodiscard]] std::size_t getSelectionStart() const;

        /** @brief Return the selection end (always >= selectionStart). */
        [[nodiscard]] std::size_t getSelectionEnd() const;

        /**
         * @brief Set an explicit selection range. Cursor moves to @p end.
         *
         * @param start Selection start character index.
         * @param end   Selection end character index.
         */
        void setSelection(std::size_t start, std::size_t end);

        /** @brief Select all text. */
        void selectAll();

        /** @brief Clear the selection without moving the cursor. */
        void clearSelection();

        /** @brief Return the currently selected text. Empty if no selection. */
        [[nodiscard]] std::string getSelectedText() const;

        // ── Editing ───────────────────────────────────────────────────────────

        /**
         * @brief Insert text at the cursor position.
         *
         * If a selection is active it is replaced. Cursor advances to
         * the end of the inserted text.
         *
         * @param text UTF-8 text to insert.
         */
        void insert(const std::string& text);

        /**
         * @brief Insert a single unicode character at the cursor position.
         * @param unicode Unicode code point to insert.
         */
        void insertChar(char32_t unicode);

        /**
         * @brief Delete the character immediately before the cursor (backspace).
         * If a selection is active, deletes the selection instead.
         */
        void backspace();

        /**
         * @brief Delete the character immediately after the cursor (delete key).
         * If a selection is active, deletes the selection instead.
         */
        void deleteForward();

        /**
         * @brief Delete all text in the buffer.
         */
        void clear();

        /**
         * @brief Replace the entire buffer content.
         * Cursor moves to end. Clears all attributes.
         *
         * @param text New UTF-8 text content.
         */
        void setText(const std::string& text);

        // ── Attributes ────────────────────────────────────────────────────────

        /**
         * @brief Apply an attribute to the current selection.
         *
         * If no selection is active, the attribute is stored as the
         * "pending" style for the next inserted character. Existing ranges
         * are split at the selection boundaries and merged where possible.
         *
         * @param attr Attribute to apply. @c start and @c end are overwritten
         *             with the current selection range.
         */
        void applyAttribute(TextAttribute attr);

        /**
         * @brief Return the effective @c TextAttribute at a given character index.
         *
         * Merges all attribute ranges that cover @p index, with later ranges
         * taking precedence over earlier ones.
         *
         * @param index Character index to query.
         * @param defaultFont   Fallback font if no attribute sets one.
         * @param defaultSize   Fallback size if no attribute sets one.
         * @param defaultColor  Fallback color if no attribute sets one.
         */
        [[nodiscard]] TextAttribute getAttributeAt(
            std::size_t     index,
            const sf::Font* defaultFont,
            unsigned int    defaultSize,
            sf::Color       defaultColor) const;

        // ── Clipboard helpers ─────────────────────────────────────────────────

        /**
         * @brief Copy the current selection to the system clipboard.
         * Has no effect if nothing is selected.
         */
        void copyToClipboard() const;

        /**
         * @brief Cut the current selection to the system clipboard.
         * Has no effect if nothing is selected.
         */
        void cutToClipboard();

        /**
         * @brief Insert the system clipboard contents at the cursor.
         */
        void pasteFromClipboard();

    private:
        std::string              _text;
        std::vector<TextAttribute> _attributes;

        std::size_t _cursor   = 0;
        std::size_t _selStart = 0;
        std::size_t _selEnd   = 0;
        bool        _hasSelection = false;

        // Pending attribute for next insert (applied when no selection active)
        TextAttribute _pendingAttr;
        bool          _hasPending = false;

        void deleteRange(std::size_t start, std::size_t end);
        void shiftAttributesAfterInsert(std::size_t pos, std::size_t count);
        void shiftAttributesAfterDelete(std::size_t start, std::size_t count);
        void splitAttributeAt(std::size_t pos);
        void mergeAdjacentAttributes();
        void normalizeAttributes();

        std::size_t findWordBoundary(std::size_t pos, int direction) const;
        std::size_t findLineStart(std::size_t pos) const;
        std::size_t findLineEnd(std::size_t pos) const;
    };

} // namespace ml

#endif // RICHTEXTBUFFER_H
