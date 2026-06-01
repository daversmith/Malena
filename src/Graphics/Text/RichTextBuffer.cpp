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
        _hasPending = false;   // moving the caret discards the pending typing style
    }

    void RichTextBuffer::moveCursor(int delta, bool extendSelection)
    {
        _hasPending = false;   // arrow navigation discards the pending typing style
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

        const std::size_t at = _cursor;
        shiftAttributesAfterInsert(_cursor, text.size());
        _text.insert(_cursor, text);
        _cursor += text.size();
        clearSelection();

        // Style the inserted text. Start from the preceding character's explicit
        // attributes (so typing continues the surrounding run — code/bold/list/
        // indent), then overlay any pending style (set by a no-selection format
        // change) which takes precedence per-field. Pending is KEPT so continued
        // typing stays styled (cleared when the caret moves — see setCursor/
        // moveCursor). Merging (rather than pending replacing inheritance) is what
        // lets e.g. Tab's pending indent ride on top of an inherited list type.
        TextAttribute inh;
        bool has = false;
        if (at > 0)
            for (const auto& a : _attributes)
                if (a.start <= at - 1 && a.end > at - 1)
                {
                    if (a.font)                  { inh.font        = a.font;        has = true; }
                    if (a.charSize.has_value())  { inh.charSize    = a.charSize;    has = true; }
                    if (a.color.has_value())     { inh.color       = a.color;       has = true; }
                    if (a.bold.has_value())      { inh.bold        = a.bold;        has = true; }
                    if (a.italic.has_value())    { inh.italic      = a.italic;      has = true; }
                    if (a.underline.has_value()) { inh.underline   = a.underline;   has = true; }
                    if (a.align.has_value())     { inh.align       = a.align;       has = true; }
                    if (a.listType.has_value())  { inh.listType    = a.listType;    has = true; }
                    if (a.indentLevel.has_value()){ inh.indentLevel = a.indentLevel; has = true; }
                }
        if (_hasPending)
        {
            const TextAttribute& p = _pendingAttr;
            if (p.font)                  { inh.font        = p.font;        has = true; }
            if (p.charSize.has_value())  { inh.charSize    = p.charSize;    has = true; }
            if (p.color.has_value())     { inh.color       = p.color;       has = true; }
            if (p.bold.has_value())      { inh.bold        = p.bold;        has = true; }
            if (p.italic.has_value())    { inh.italic      = p.italic;      has = true; }
            if (p.underline.has_value()) { inh.underline   = p.underline;   has = true; }
            if (p.align.has_value())     { inh.align       = p.align;       has = true; }
            if (p.listType.has_value())  { inh.listType    = p.listType;    has = true; }
            if (p.indentLevel.has_value()){ inh.indentLevel = p.indentLevel; has = true; }
        }
        if (has)
            applyAttributeRange(at, at + text.size(), inh);
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
            // Merge into the pending style (applied to the next inserted text),
            // so several no-selection changes (font + size + bold) accumulate
            // instead of the last one replacing the rest.
            if (!_hasPending) _pendingAttr = TextAttribute{};
            if (attr.font)                  _pendingAttr.font      = attr.font;
            if (attr.charSize.has_value())  _pendingAttr.charSize  = attr.charSize;
            if (attr.color.has_value())     _pendingAttr.color     = attr.color;
            if (attr.bold.has_value())      _pendingAttr.bold      = attr.bold;
            if (attr.italic.has_value())    _pendingAttr.italic    = attr.italic;
            if (attr.underline.has_value()) _pendingAttr.underline = attr.underline;
            if (attr.align.has_value())     _pendingAttr.align     = attr.align;
            if (attr.listType.has_value())  _pendingAttr.listType  = attr.listType;
            if (attr.indentLevel.has_value())_pendingAttr.indentLevel = attr.indentLevel;
            _hasPending = true;
            return;
        }

        applyAttributeRange(getSelectionStart(), getSelectionEnd(), attr);
    }

    void RichTextBuffer::applyAttributeRange(std::size_t start, std::size_t end,
                                             TextAttribute attr)
    {
        end = std::min(end, _text.size());
        if (start >= end) return;

        // Split existing attributes at boundaries so ranges are either fully
        // inside [start, end) or fully outside.
        splitAttributeAt(start);
        splitAttributeAt(end);

        // Overlay only the fields the caller actually set, preserving the rest —
        // so bold + italic + underline + color accumulate instead of replacing.
        auto overlay = [](TextAttribute& dst, const TextAttribute& src) {
            if (src.font)                  dst.font      = src.font;
            if (src.charSize.has_value())  dst.charSize  = src.charSize;
            if (src.color.has_value())     dst.color     = src.color;
            if (src.bold.has_value())      dst.bold      = src.bold;
            if (src.italic.has_value())    dst.italic    = src.italic;
            if (src.underline.has_value()) dst.underline = src.underline;
            if (src.align.has_value())     dst.align     = src.align;
            if (src.listType.has_value())  dst.listType  = src.listType;
            if (src.indentLevel.has_value())dst.indentLevel = src.indentLevel;
        };

        // Merge into existing ranges inside [start, end); record their coverage.
        std::vector<std::pair<std::size_t, std::size_t>> covered;
        for (auto& a : _attributes)
            if (a.start >= start && a.end <= end)
            {
                overlay(a, attr);
                covered.emplace_back(a.start, a.end);
            }

        // Fill any uncovered gaps with a fresh range carrying just the new attr.
        std::sort(covered.begin(), covered.end());
        std::size_t cur = start;
        for (const auto& [s, e] : covered)
        {
            if (s > cur) { TextAttribute g = attr; g.start = cur; g.end = s; _attributes.push_back(g); }
            cur = std::max(cur, e);
        }
        if (cur < end) { TextAttribute g = attr; g.start = cur; g.end = end; _attributes.push_back(g); }

        normalizeAttributes();
    }

    void RichTextBuffer::clearAttributes()
    {
        _attributes.clear();
        _hasPending = false;
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
                if (attr.align.has_value())    result.align    = attr.align;
                if (attr.listType.has_value()) result.listType = attr.listType;
                if (attr.indentLevel.has_value()) result.indentLevel = attr.indentLevel;
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
