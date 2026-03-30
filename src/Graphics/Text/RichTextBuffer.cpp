// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Text/RichTextBuffer.h>
#include <SFML/Window/Clipboard.hpp>
#include <algorithm>
#include <cctype>

namespace ml
{
    // ── Buffer access ─────────────────────────────────────────────────────────

    const std::string& RichTextBuffer::getText() const { return _text; }
    std::size_t        RichTextBuffer::size()    const { return _text.size(); }
    bool               RichTextBuffer::empty()   const { return _text.empty(); }

    const std::vector<TextAttribute>& RichTextBuffer::getAttributes() const
    {
        return _attributes;
    }

    // ── Cursor ────────────────────────────────────────────────────────────────

    std::size_t RichTextBuffer::getCursor() const { return _cursor; }

    void RichTextBuffer::setCursor(std::size_t pos)
    {
        _cursor = std::min(pos, _text.size());
        clearSelection();
    }

    void RichTextBuffer::moveCursor(int delta, bool extendSelection)
    {
        const std::size_t newPos = (delta < 0)
            ? (_cursor >= static_cast<std::size_t>(-delta) ? _cursor + delta : 0)
            : std::min(_cursor + static_cast<std::size_t>(delta), _text.size());

        if (extendSelection)
        {
            if (!_hasSelection)
            {
                _selStart = _cursor;
                _selEnd   = _cursor;
                _hasSelection = true;
            }
            _cursor = newPos;
            _selEnd = newPos;
            // Normalize so selStart <= selEnd
            if (_selStart > _selEnd) std::swap(_selStart, _selEnd);
        }
        else
        {
            if (_hasSelection && delta != 0)
            {
                // Jump to selection boundary without moving
                _cursor = (delta < 0) ? _selStart : _selEnd;
                clearSelection();
            }
            else
            {
                _cursor = newPos;
                clearSelection();
            }
        }
    }

    void RichTextBuffer::moveCursorByWord(int direction, bool extendSelection)
    {
        const std::size_t newPos = findWordBoundary(_cursor, direction);
        if (extendSelection)
        {
            if (!_hasSelection) { _selStart = _cursor; _selEnd = _cursor; _hasSelection = true; }
            _cursor = newPos;
            _selEnd = newPos;
            if (_selStart > _selEnd) std::swap(_selStart, _selEnd);
        }
        else
        {
            _cursor = newPos;
            clearSelection();
        }
    }

    void RichTextBuffer::moveCursorToLineStart(bool extendSelection)
    {
        const std::size_t newPos = findLineStart(_cursor);
        if (extendSelection)
        {
            if (!_hasSelection) { _selStart = _cursor; _selEnd = _cursor; _hasSelection = true; }
            _cursor = newPos;
            if (_cursor < _selStart) _selStart = _cursor;
            else _selEnd = _cursor; // shouldn't happen going to start
            if (_selStart > _selEnd) std::swap(_selStart, _selEnd);
        }
        else
        {
            _cursor = newPos;
            clearSelection();
        }
    }

    void RichTextBuffer::moveCursorToLineEnd(bool extendSelection)
    {
        const std::size_t newPos = findLineEnd(_cursor);
        if (extendSelection)
        {
            if (!_hasSelection) { _selStart = _cursor; _selEnd = _cursor; _hasSelection = true; }
            _cursor = newPos;
            _selEnd = std::max(_selEnd, newPos);
            if (_selStart > _selEnd) std::swap(_selStart, _selEnd);
        }
        else
        {
            _cursor = newPos;
            clearSelection();
        }
    }

    // ── Selection ─────────────────────────────────────────────────────────────

    bool        RichTextBuffer::hasSelection()     const { return _hasSelection && _selStart != _selEnd; }
    std::size_t RichTextBuffer::getSelectionStart()const { return std::min(_selStart, _selEnd); }
    std::size_t RichTextBuffer::getSelectionEnd()  const { return std::max(_selStart, _selEnd); }

    void RichTextBuffer::setSelection(std::size_t start, std::size_t end)
    {
        _selStart     = std::min(start, _text.size());
        _selEnd       = std::min(end,   _text.size());
        _cursor       = _selEnd;
        _hasSelection = (_selStart != _selEnd);
    }

    void RichTextBuffer::selectAll()
    {
        _selStart     = 0;
        _selEnd       = _text.size();
        _cursor       = _selEnd;
        _hasSelection = !_text.empty();
    }

    void RichTextBuffer::clearSelection()
    {
        _selStart = _selEnd = _cursor;
        _hasSelection = false;
    }

    std::string RichTextBuffer::getSelectedText() const
    {
        if (!hasSelection()) return {};
        return _text.substr(getSelectionStart(), getSelectionEnd() - getSelectionStart());
    }

    // ── Editing ───────────────────────────────────────────────────────────────

    void RichTextBuffer::insert(const std::string& text)
    {
        if (text.empty()) return;

        if (hasSelection())
            deleteRange(getSelectionStart(), getSelectionEnd());

        shiftAttributesAfterInsert(_cursor, text.size());
        _text.insert(_cursor, text);
        _cursor += text.size();
        clearSelection();
    }

    void RichTextBuffer::insertChar(char32_t unicode)
    {
        // Convert unicode codepoint to UTF-8
        std::string utf8;
        if (unicode < 0x80)
        {
            utf8 += static_cast<char>(unicode);
        }
        else if (unicode < 0x800)
        {
            utf8 += static_cast<char>(0xC0 | (unicode >> 6));
            utf8 += static_cast<char>(0x80 | (unicode & 0x3F));
        }
        else if (unicode < 0x10000)
        {
            utf8 += static_cast<char>(0xE0 | (unicode >> 12));
            utf8 += static_cast<char>(0x80 | ((unicode >> 6) & 0x3F));
            utf8 += static_cast<char>(0x80 | (unicode & 0x3F));
        }
        else
        {
            utf8 += static_cast<char>(0xF0 | (unicode >> 18));
            utf8 += static_cast<char>(0x80 | ((unicode >> 12) & 0x3F));
            utf8 += static_cast<char>(0x80 | ((unicode >> 6) & 0x3F));
            utf8 += static_cast<char>(0x80 | (unicode & 0x3F));
        }
        insert(utf8);
    }

    void RichTextBuffer::backspace()
    {
        if (hasSelection())
        {
            deleteRange(getSelectionStart(), getSelectionEnd());
            return;
        }
        if (_cursor > 0)
        {
            deleteRange(_cursor - 1, _cursor);
        }
    }

    void RichTextBuffer::deleteForward()
    {
        if (hasSelection())
        {
            deleteRange(getSelectionStart(), getSelectionEnd());
            return;
        }
        if (_cursor < _text.size())
            deleteRange(_cursor, _cursor + 1);
    }

    void RichTextBuffer::clear()
    {
        _text.clear();
        _attributes.clear();
        _cursor = 0;
        clearSelection();
    }

    void RichTextBuffer::setText(const std::string& text)
    {
        _text = text;
        _attributes.clear();
        _cursor = _text.size();
        clearSelection();
    }

    // ── Attributes ────────────────────────────────────────────────────────────

    void RichTextBuffer::applyAttribute(TextAttribute attr)
    {
        if (!hasSelection())
        {
            // Store as pending for next insert
            _pendingAttr = attr;
            _hasPending  = true;
            return;
        }

        attr.start = getSelectionStart();
        attr.end   = getSelectionEnd();

        // Split existing attributes at boundaries
        splitAttributeAt(attr.start);
        splitAttributeAt(attr.end);

        // Remove ranges fully inside the new range
        _attributes.erase(
            std::remove_if(_attributes.begin(), _attributes.end(),
                [&](const TextAttribute& a){
                    return a.start >= attr.start && a.end <= attr.end;
                }),
            _attributes.end());

        _attributes.push_back(attr);
        normalizeAttributes();
    }

    TextAttribute RichTextBuffer::getAttributeAt(
        std::size_t     index,
        const sf::Font* defaultFont,
        unsigned int    defaultSize,
        sf::Color       defaultColor) const
    {
        TextAttribute result;
        result.font     = defaultFont;
        result.charSize = defaultSize;
        result.color    = defaultColor;

        for (const auto& attr : _attributes)
        {
            if (attr.start <= index && attr.end > index)
            {
                if (attr.font)                result.font     = attr.font;
                if (attr.charSize.has_value()) result.charSize = attr.charSize;
                if (attr.color.has_value())    result.color    = attr.color;
                if (attr.bold.has_value())     result.bold     = attr.bold;
                if (attr.italic.has_value())   result.italic   = attr.italic;
                if (attr.underline.has_value())result.underline= attr.underline;
            }
        }
        return result;
    }

    // ── Clipboard ────────────────────────────────────────────────────────────

    void RichTextBuffer::copyToClipboard() const
    {
        if (hasSelection())
            sf::Clipboard::setString(getSelectedText());
    }

    void RichTextBuffer::cutToClipboard()
    {
        if (hasSelection())
        {
            sf::Clipboard::setString(getSelectedText());
            deleteRange(getSelectionStart(), getSelectionEnd());
        }
    }

    void RichTextBuffer::pasteFromClipboard()
    {
        const std::string pasted = sf::Clipboard::getString();
        if (!pasted.empty())
            insert(pasted);
    }

    // ── Private helpers ───────────────────────────────────────────────────────

    void RichTextBuffer::deleteRange(std::size_t start, std::size_t end)
    {
        if (start >= end) return;
        end = std::min(end, _text.size());

        shiftAttributesAfterDelete(start, end - start);
        _text.erase(start, end - start);
        _cursor = start;  // cursor lands at start of deleted range
        clearSelection();
    }

    void RichTextBuffer::shiftAttributesAfterInsert(std::size_t pos, std::size_t count)
    {
        for (auto& attr : _attributes)
        {
            if (attr.start >= pos) attr.start += count;
            if (attr.end   >= pos) attr.end   += count;
        }
    }

    void RichTextBuffer::shiftAttributesAfterDelete(std::size_t start, std::size_t count)
    {
        for (auto& attr : _attributes)
        {
            // Clamp end into deleted range
            if (attr.end > start)
                attr.end = (attr.end >= start + count) ? attr.end - count : start;

            // Clamp start into deleted range
            if (attr.start > start)
                attr.start = (attr.start >= start + count) ? attr.start - count : start;
        }

        // Remove zero-length ranges
        _attributes.erase(
            std::remove_if(_attributes.begin(), _attributes.end(),
                [](const TextAttribute& a){ return a.start >= a.end; }),
            _attributes.end());
    }

    void RichTextBuffer::splitAttributeAt(std::size_t pos)
    {
        std::vector<TextAttribute> extra;
        for (auto& attr : _attributes)
        {
            if (attr.start < pos && attr.end > pos)
            {
                TextAttribute right = attr;
                right.start  = pos;
                attr.end     = pos;
                extra.push_back(right);
            }
        }
        for (auto& e : extra)
            _attributes.push_back(e);
    }

    void RichTextBuffer::normalizeAttributes()
    {
        // Sort by start
        std::sort(_attributes.begin(), _attributes.end(),
            [](const TextAttribute& a, const TextAttribute& b){ return a.start < b.start; });

        // Remove zero-length
        _attributes.erase(
            std::remove_if(_attributes.begin(), _attributes.end(),
                [](const TextAttribute& a){ return a.start >= a.end; }),
            _attributes.end());
    }

    std::size_t RichTextBuffer::findWordBoundary(std::size_t pos, int direction) const
    {
        if (_text.empty()) return 0;

        if (direction > 0)
        {
            // Skip non-whitespace, then whitespace
            std::size_t i = pos;
            while (i < _text.size() && !std::isspace(static_cast<unsigned char>(_text[i]))) ++i;
            while (i < _text.size() &&  std::isspace(static_cast<unsigned char>(_text[i]))) ++i;
            return i;
        }
        else
        {
            // Skip whitespace backwards, then non-whitespace
            std::size_t i = pos;
            while (i > 0 && std::isspace(static_cast<unsigned char>(_text[i - 1]))) --i;
            while (i > 0 && !std::isspace(static_cast<unsigned char>(_text[i - 1]))) --i;
            return i;
        }
    }

    std::size_t RichTextBuffer::findLineStart(std::size_t pos) const
    {
        if (pos == 0) return 0;
        std::size_t i = pos;
        while (i > 0 && _text[i - 1] != '\n') --i;
        return i;
    }

    std::size_t RichTextBuffer::findLineEnd(std::size_t pos) const
    {
        std::size_t i = pos;
        while (i < _text.size() && _text[i] != '\n') ++i;
        return i;
    }

} // namespace ml
