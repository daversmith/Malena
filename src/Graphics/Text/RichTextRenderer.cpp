// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Text/RichTextRenderer.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
#include <vector>

namespace
{
    // Lowercase alphabetic ordinal: 1→a, 2→b … 26→z, 27→aa, 28→ab …
    std::string toAlpha(int n)
    {
        std::string s;
        while (n > 0) { int r = (n - 1) % 26; s.insert(s.begin(), char('a' + r)); n = (n - 1) / 26; }
        return s.empty() ? "a" : s;
    }

    // Lowercase roman numeral: 1→i, 4→iv, 9→ix …
    std::string toRoman(int n)
    {
        static const int    val[] = { 1000,900,500,400,100,90,50,40,10,9,5,4,1 };
        static const char*   sym[] = { "m","cm","d","cd","c","xc","l","xl","x","ix","v","iv","i" };
        std::string s;
        for (int i = 0; i < 13 && n > 0; ++i)
            while (n >= val[i]) { s += sym[i]; n -= val[i]; }
        return s.empty() ? "i" : s;
    }

    // Numbered marker for a 1-based count at a nesting level: decimal / alpha /
    // roman, cycling every 3 levels (1. → a. → i. → 1. …).
    std::string numberMarker(int count, int level)
    {
        switch (level % 3)
        {
            case 1:  return toAlpha(count) + ".";
            case 2:  return toRoman(count) + ".";
            default: return std::to_string(count) + ".";
        }
    }

    // Horizontal ADVANCE of a positioned sf::Text run (pen-to-pen), not its visual
    // bounding box. getGlobalBounds().size.x omits side bearings and is ~0 for a
    // space, which packs glyphs too tightly; the pen advance is the correct width
    // for laying the next segment.
    float segAdvance(const sf::Text& t)
    {
        const std::size_t n = t.getString().getSize();
        if (n == 0) return 0.f;
        return t.findCharacterPos(n).x - t.findCharacterPos(0).x;
    }

    // Bullet glyph (UTF-8) for a nesting level, cycling every 3: • → ◦ → ▪.
    const char* bulletMarker(int level)
    {
        switch (level % 3)
        {
            case 1:  return "\xE2\x97\xA6";   // ◦ white bullet
            case 2:  return "\xE2\x96\xAA";   // ▪ small black square
            default: return "\xE2\x80\xA2";   // • bullet
        }
    }
}

namespace ml
{
    RichTextRenderer::RichTextRenderer(const RichTextBuffer& buffer,
                                       const sf::Font&       defaultFont,
                                       unsigned int          defaultSize,
                                       sf::Color             defaultColor)
        : _buffer(buffer),
          _defaultFont(&defaultFont),
          _defaultSize(defaultSize),
          _defaultColor(defaultColor)
    {}

    void RichTextRenderer::setOrigin(const sf::Vector2f& origin)
    {
        _origin = origin;
        reflow();  // reposition only — never recreates sf::Text objects
    }

    void RichTextRenderer::setMaxWidth(float width)
    {
        _maxWidth = width;
        rebuild();
    }

    // ── Rebuild ───────────────────────────────────────────────────────────────

    void RichTextRenderer::rebuild()
    {
        _lines.clear();

        if (_buffer.empty())
        {
            RenderedLine empty;
            empty.y         = _origin.y;
            empty.height    = static_cast<float>(_defaultSize);
            empty.charStart = 0;
            empty.charEnd   = 0;
            _lines.push_back(std::move(empty));
            return;
        }

        buildSegments();
        computeListMarkers();
        layoutLines();
    }

    unsigned int RichTextRenderer::emptyLineSize(std::size_t charStart) const
    {
        if (charStart > 0)
            return _buffer.getAttributeAt(charStart - 1, _defaultFont, _defaultSize, _defaultColor)
                       .charSize.value_or(_defaultSize);
        return _defaultSize;
    }

    void RichTextRenderer::computeListMarkers()
    {
        constexpr float kStep = 26.f;   // px per indent level / list gutter

        // counters[L] = running ordinal of the current numbered run at level L.
        // Outdenting drops deeper levels (so re-entering restarts at 1); a
        // non-list paragraph clears the whole stack.
        std::vector<int> counters;
        for (auto& line : _lines)
        {
            // An empty paragraph (e.g. a fresh line just created by Enter) owns no
            // chars, so the paragraph style lives on the preceding newline —
            // inherit it so the bullet/number/indent appears before any typing.
            const bool emptyTrailing =
                (line.charStart == line.charEnd && line.charStart > 0);
            const std::size_t probe = emptyTrailing ? line.charStart - 1 : line.charStart;
            const TextAttribute pa = _buffer.getAttributeAt(
                probe, _defaultFont, _defaultSize, _defaultColor);

            int lt    = pa.listType.value_or(0);
            int level = std::max(0, pa.indentLevel.value_or(0));

            // A just-Tabbed / just-toggled empty paragraph holds its new style in
            // the buffer's pending attribute (no chars to carry it yet). Preview it
            // on the caret's line so the indent/marker shows before any typing.
            if (line.segments.empty() && _buffer.hasPendingAttribute())
            {
                const std::size_t cur = _buffer.getCursor();
                if (cur >= line.charStart && cur <= line.charEnd)
                {
                    const TextAttribute& pend = _buffer.getPendingAttribute();
                    if (pend.listType.has_value())    lt    = pend.listType.value();
                    if (pend.indentLevel.has_value()) level = std::max(0, pend.indentLevel.value());
                }
            }
            line.listType = lt;

            if (lt == 0)
            {
                counters.clear();
                line.marker.clear();
                line.indent = static_cast<float>(level) * kStep;   // plain indent
                continue;
            }

            // Size the counter stack to this level; shrinking (outdent) discards
            // the deeper counters so a returning sublevel renumbers from 1.
            if (static_cast<int>(counters.size()) > level + 1)
                counters.resize(level + 1);
            if (static_cast<int>(counters.size()) < level + 1)
                counters.resize(level + 1, 0);

            if (lt == 2) { line.marker = numberMarker(++counters[level], level); }
            else         { line.marker = bulletMarker(level); }

            // List text hangs one gutter past its marker; the marker sits at the
            // indent-level offset.
            line.indent = static_cast<float>(level + 1) * kStep;
        }
    }

    void RichTextRenderer::reflow()
    {
        // Reposition existing sf::Text objects without recreating them.
        // Safe to call every frame — no atlas stress.
        if (_lines.empty()) return;

        float y = _origin.y;
        for (auto& line : _lines)
        {
            line.y = y;

            // Baseline alignment (must match layoutLines): the tallest segment
            // drives the baseline; smaller segments shift down to share it.
            // Empty lines size to what you'd type there; otherwise the actual max.
            float maxSize = line.segments.empty()
                ? static_cast<float>(emptyLineSize(line.charStart)) : 0.f;
            for (const auto& seg : line.segments)
                maxSize = std::max(maxSize,
                    static_cast<float>(seg.sfText.getCharacterSize()));
            const float rowStep = maxSize * 1.2f;

            auto baselineY = [&](const RenderedSegment& seg, float top) {
                return top + (maxSize - static_cast<float>(seg.sfText.getCharacterSize()));
            };

            const float left = _origin.x + line.indent;
            if (line.listType != 0)
                line.markerPos = { _origin.x + line.indent - 26.f,
                                   y + (maxSize - static_cast<float>(_defaultSize)) };

            float x         = left;
            float rowStartY = y;
            int   wrapCount = 0;

            for (auto& seg : line.segments)
            {
                seg.position = {x, baselineY(seg, y)};
                seg.sfText.setPosition(seg.position);

                const float segWidth = segAdvance(seg.sfText);

                if (_maxWidth > 0.f && x + segWidth - left > _maxWidth - line.indent && x > left)
                {
                    y += rowStep;
                    x  = left;
                    ++wrapCount;
                    seg.position = {x, baselineY(seg, y)};
                    seg.sfText.setPosition(seg.position);
                }

                x += segAdvance(seg.sfText);
            }

            alignLineRows(line, maxSize);   // apply paragraph alignment

            // Keep line.height consistent with layoutLines()
            line.rowHeight = rowStep;
            line.height    = rowStep * static_cast<float>(wrapCount + 1);
            y = rowStartY + line.height;
        }
    }

    void RichTextRenderer::buildSegments()
    {
        const std::string& text = _buffer.getText();
        if (text.empty()) return;

        std::vector<std::size_t> boundaries;
        boundaries.push_back(0);
        boundaries.push_back(text.size());

        for (const auto& attr : _buffer.getAttributes())
        {
            boundaries.push_back(attr.start);
            boundaries.push_back(attr.end);
        }

        for (std::size_t i = 0; i < text.size(); ++i)
            if (text[i] == '\n') { boundaries.push_back(i); boundaries.push_back(i + 1); }

        // When wrapping is enabled, split at every character so layoutLines()
        // can wrap at any position — handles both word-wrapped and continuous
        // text (no spaces). Boundaries adjacent to newlines are skipped since
        // newlines are already split points.
        if (_maxWidth > 0.f)
        {
            for (std::size_t i = 1; i < text.size(); ++i)
                if (text[i - 1] != '\n' && text[i] != '\n')
                    boundaries.push_back(i);
        }

        std::sort(boundaries.begin(), boundaries.end());
        boundaries.erase(std::unique(boundaries.begin(), boundaries.end()), boundaries.end());

        RenderedLine currentLine;
        currentLine.y         = _origin.y;
        currentLine.charStart = 0;

        for (std::size_t i = 0; i + 1 < boundaries.size(); ++i)
        {
            const std::size_t segStart = boundaries[i];
            const std::size_t segEnd   = boundaries[i + 1];
            if (segStart >= segEnd) continue;

            const std::string segText = text.substr(segStart, segEnd - segStart);

            if (segText == "\n")
            {
                currentLine.charEnd = segEnd;
                _lines.push_back(std::move(currentLine));
                currentLine          = RenderedLine{};
                currentLine.y        = 0.f;
                currentLine.charStart= segEnd;
                continue;
            }

            const TextAttribute attr = _buffer.getAttributeAt(
                segStart, _defaultFont, _defaultSize, _defaultColor);

            sf::Text sfText = makeText(segText, attr);

            currentLine.segments.emplace_back(
                std::move(sfText),
                segStart,
                segEnd,
                sf::Vector2f{}
            );
        }

        currentLine.charEnd = text.size();
        _lines.push_back(std::move(currentLine));
    }

    void RichTextRenderer::layoutLines()
    {
        float y = _origin.y;

        for (auto& line : _lines)
        {
            line.y = y;

            // The tallest segment drives the shared baseline. Each segment is
            // shifted DOWN by (maxSize - its size) so all sit on one baseline
            // (bottom/baseline alignment, like a word processor) instead of
            // aligning at the top. lineHeight leaves room for descenders.
            // Only EMPTY lines fall back to a default — and even then to the size
            // text typed there would take (preceding char), not the global default,
            // so a blank line after small text isn't oversized. A non-empty line
            // uses its actual max so small text doesn't inherit the default height.
            float maxSize = line.segments.empty()
                ? static_cast<float>(emptyLineSize(line.charStart)) : 0.f;
            for (const auto& seg : line.segments)
                maxSize = std::max(maxSize,
                    static_cast<float>(seg.sfText.getCharacterSize()));
            const float lineHeight = maxSize * 1.2f;
            line.height = lineHeight;

            auto baselineY = [&](const RenderedSegment& seg, float top) {
                return top + (maxSize - static_cast<float>(seg.sfText.getCharacterSize()));
            };

            // List paragraphs indent their text and place a marker in the gutter,
            // baseline-aligned with the first row at the default text size.
            const float left = _origin.x + line.indent;
            if (line.listType != 0)
                line.markerPos = { _origin.x + line.indent - 26.f,
                                   y + (maxSize - static_cast<float>(_defaultSize)) };

            float x        = left;
            float rowStartY = y;
            int   wrapCount = 0;

            for (auto& seg : line.segments)
            {
                seg.position = {x, baselineY(seg, y)};
                seg.sfText.setPosition(seg.position);

                const float segWidth = segAdvance(seg.sfText);

                if (_maxWidth > 0.f && x + segWidth - left > _maxWidth - line.indent && x > left)
                {
                    y += lineHeight;
                    x  = left;
                    ++wrapCount;
                    seg.position = {x, baselineY(seg, y)};
                    seg.sfText.setPosition(seg.position);
                }

                x += segAdvance(seg.sfText);
            }

            alignLineRows(line, maxSize);   // apply paragraph alignment

            // line.height must cover ALL wrapped rows, not just one.
            // getTotalHeight() uses this to compute total scrollable content.
            line.rowHeight = lineHeight;
            line.height    = lineHeight * static_cast<float>(wrapCount + 1);

            y = rowStartY + line.height;
        }
    }

    void RichTextRenderer::alignLineRows(RenderedLine& line, float maxSize)
    {
        if (line.segments.empty() || _maxWidth <= 0.f) return;

        const int align = _buffer.getAttributeAt(
            line.charStart, _defaultFont, _defaultSize, _defaultColor).align.value_or(0);
        if (align == 0) return;   // left — nothing to do

        // Segments are baseline-shifted (different y per size within a row), so
        // group by the computed row TOP, then shift each row by its alignment.
        // List paragraphs align within their indented column (left of the text).
        const float left  = _origin.x + line.indent;
        const float avail = _maxWidth - line.indent;
        auto rowTopOf = [&](const RenderedSegment& sg) {
            return sg.position.y - (maxSize - static_cast<float>(sg.sfText.getCharacterSize()));
        };

        std::size_t i = 0;
        while (i < line.segments.size())
        {
            const float rowTop = rowTopOf(line.segments[i]);
            std::size_t j = i;
            float maxX = left;
            while (j < line.segments.size() &&
                   std::abs(rowTopOf(line.segments[j]) - rowTop) < 0.5f)
            {
                const auto& sg = line.segments[j];
                maxX = std::max(maxX, sg.position.x + segAdvance(sg.sfText));
                ++j;
            }
            const float rowW = maxX - left;
            float off = (align == 1) ? (avail - rowW) * 0.5f : (avail - rowW);
            if (off < 0.f) off = 0.f;
            if (off > 0.f)
                for (std::size_t k = i; k < j; ++k)
                {
                    line.segments[k].position.x += off;
                    line.segments[k].sfText.setPosition(line.segments[k].position);
                }
            i = j;
        }
    }

    sf::Text RichTextRenderer::makeText(const std::string&   str,
                                        const TextAttribute& attr) const
    {
        const sf::Font& font = attr.font ? *attr.font : *_defaultFont;
        unsigned int    size = attr.charSize.value_or(_defaultSize);
        sf::Color       col  = attr.color.value_or(_defaultColor);

        sf::Text t(font, str, size);
        t.setFillColor(col);

        auto combineStyle = [](sf::Text::Style a, sf::Text::Style b) {
            return static_cast<sf::Text::Style>(static_cast<int>(a) | static_cast<int>(b));
        };

        sf::Text::Style style = sf::Text::Style::Regular;
        if (attr.bold.value_or(false))      style = combineStyle(style, sf::Text::Style::Bold);
        if (attr.italic.value_or(false))    style = combineStyle(style, sf::Text::Style::Italic);
        if (attr.underline.value_or(false)) style = combineStyle(style, sf::Text::Style::Underlined);
        t.setStyle(style);

        return t;
    }

    // ── Drawing ───────────────────────────────────────────────────────────────

    void RichTextRenderer::draw(sf::RenderTarget&       target,
                                const sf::RenderStates& states) const
    {
        for (const auto& line : _lines)
        {
            if (line.listType != 0 && !line.marker.empty())
            {
                sf::Text mk = makeText(line.marker, TextAttribute{});
                // sf::Text(std::string) decodes as Latin-1, turning the 3-byte
                // UTF-8 bullet into tofu — re-set it as a decoded UTF-8 string.
                mk.setString(sf::String::fromUtf8(line.marker.begin(), line.marker.end()));
                mk.setPosition(line.markerPos);
                target.draw(mk, states);
            }
            for (const auto& seg : line.segments)
                target.draw(seg.sfText, states);
        }
    }

    void RichTextRenderer::drawSelection(sf::RenderTarget&       target,
                                         const sf::RenderStates& states,
                                         std::size_t             selStart,
                                         std::size_t             selEnd,
                                         const sf::Color&        color) const
    {
        if (selStart >= selEnd) return;

        const std::size_t startLine = lineIndexForChar(selStart);
        const std::size_t endLine   = lineIndexForChar(selEnd);

        auto rowH = [&](std::size_t li) -> float {
            const auto& ln = _lines[li];
            return ln.rowHeight > 0.f ? ln.rowHeight : static_cast<float>(_defaultSize);
        };
        const float lineRight = _origin.x + (_maxWidth > 0.f ? _maxWidth : 10000.f);

        auto drawRect = [&](float x, float y, float w, float h) {
            if (w <= 0.f) return;
            sf::RectangleShape r({w, h});
            r.setPosition({x, y});
            r.setFillColor(color);
            target.draw(r, states);
        };

        // Highlight each selected logical line, row by row so WRAPPED rows are
        // covered too (the previous code only drew a single row for the first and
        // last lines, leaving wrapped continuations un-highlighted).
        for (std::size_t li = startLine; li <= endLine && li < _lines.size(); ++li)
        {
            const auto& line = _lines[li];
            const std::size_t a = std::max(selStart, line.charStart);
            const std::size_t b = std::min(selEnd,   line.charEnd);
            if (a > b) continue;

            const float rh    = rowH(li);
            const sf::Vector2f pa = charIndexToPosition(a);

            // Does the selection continue past this line (its newline is selected)?
            // If so the line's tail rows run full width to the right edge.
            const bool extend = (selEnd > line.charEnd);

            float lastRowY, lastRightX;
            if (extend)
            {
                lastRowY   = line.y + line.height - rh;   // bottom visual row
                lastRightX = lineRight;
            }
            else
            {
                const sf::Vector2f pb = charIndexToPosition(b);
                // Guard against charIndexToPosition jumping to the next line when b
                // lands exactly on this line's trailing newline.
                if (pb.y >= line.y + line.height) { lastRowY = line.y + line.height - rh; lastRightX = lineRight; }
                else                              { lastRowY = pb.y;                      lastRightX = pb.x; }
            }

            if (std::abs(pa.y - lastRowY) < 0.5f)
            {
                drawRect(pa.x, pa.y, lastRightX - pa.x, rh);    // single visual row
            }
            else
            {
                drawRect(pa.x, pa.y, lineRight - pa.x, rh);     // first row: caret → right
                for (float y = pa.y + rh; y < lastRowY - 0.5f; y += rh)
                    drawRect(_origin.x, y, lineRight - _origin.x, rh);   // middle rows
                drawRect(_origin.x, lastRowY, lastRightX - _origin.x, rh); // last row
            }
        }
    }

    void RichTextRenderer::drawCursor(sf::RenderTarget&       target,
                                      const sf::RenderStates& states,
                                      std::size_t             charIndex,
                                      const sf::Color&        color,
                                      float                   width) const
    {
        // Resolve BOTH the caret position and its height from the SAME segment,
        // so the caret rests exactly on that run's baseline (taking position from
        // one run and height from another makes it float above/below the line).
        sf::Vector2f pos = _origin;
        float        h   = static_cast<float>(_defaultSize);

        for (std::size_t li = 0; li < _lines.size(); ++li)
        {
            const auto& line = _lines[li];
            if (charIndex < line.charStart || charIndex > line.charEnd) continue;
            if (charIndex == line.charEnd
                && li + 1 < _lines.size()
                && _lines[li + 1].charStart == charIndex)
                continue;

            const RenderedSegment* hit = nullptr;
            for (const auto& seg : line.segments)
                if (charIndex >= seg.bufStart && charIndex <= seg.bufEnd) { hit = &seg; break; }
            if (!hit && !line.segments.empty()) hit = &line.segments.back();

            if (hit)
            {
                const std::size_t local =
                    std::min(charIndex - hit->bufStart, hit->sfText.getString().getSize());
                pos = hit->sfText.findCharacterPos(local);
                h   = static_cast<float>(hit->sfText.getCharacterSize());
            }
            else
            {
                // Empty line (no segments) — sit the caret in the indented column
                // so a fresh list item's caret rests right after its marker, and
                // size it to what you'd type there (matches the line height).
                pos = { _origin.x + line.indent, line.y };
                h   = static_cast<float>(emptyLineSize(line.charStart));
            }
            break;
        }

        sf::RectangleShape cursor({width, h});
        cursor.setPosition(pos);
        cursor.setFillColor(color);
        target.draw(cursor, states);
    }

    // ── Coordinate mapping ────────────────────────────────────────────────────

    sf::Vector2f RichTextRenderer::charIndexToPosition(std::size_t index) const
    {
        if (_lines.empty()) return _origin;

        for (std::size_t li = 0; li < _lines.size(); ++li)
        {
            const auto& line = _lines[li];
            if (index < line.charStart || index > line.charEnd) continue;

            // When index is exactly at this line's end and the next line starts
            // at the same index (i.e., cursor is right after a '\n'), the cursor
            // belongs at the start of the next line, not the end of this one.
            if (index == line.charEnd
                && li + 1 < _lines.size()
                && _lines[li + 1].charStart == index)
                continue;

            for (const auto& seg : line.segments)
            {
                if (index >= seg.bufStart && index <= seg.bufEnd)
                {
                    const std::size_t local = index - seg.bufStart;
                    return seg.sfText.findCharacterPos(local);
                }
            }

            if (!line.segments.empty())
            {
                const auto& last = line.segments.back();
                return last.sfText.findCharacterPos(last.sfText.getString().getSize());
            }
            return {_origin.x + line.indent, line.y};
        }

        const auto& lastLine = _lines.back();
        if (!lastLine.segments.empty())
        {
            const auto& lastSeg = lastLine.segments.back();
            return lastSeg.sfText.findCharacterPos(lastSeg.sfText.getString().getSize());
        }
        return {_origin.x + lastLine.indent, lastLine.y};
    }

    std::size_t RichTextRenderer::positionToCharIndex(const sf::Vector2f& pos) const
    {
        if (_lines.empty()) return 0;

        // ── Step 1: find the logical line that contains pos.y ─────────────────
        const RenderedLine* targetLine = &_lines.front();
        for (const auto& line : _lines)
        {
            if (pos.y >= line.y && pos.y < line.y + line.height)
            {
                targetLine = &line;
                break;
            }
            if (pos.y >= line.y)
                targetLine = &line;
        }

        if (targetLine->segments.empty())
            return targetLine->charStart;

        // ── Step 2: find the correct visual row within a wrapped logical line ──
        // When word wrap places segments on multiple y positions within one
        // RenderedLine, clicking on row N must only match segments on row N.
        // Without this, X-only distance matching picks wrong characters from
        // adjacent visual rows.
        float targetRowY     = targetLine->y;
        float closestRowDist = std::numeric_limits<float>::max();

        for (const auto& seg : targetLine->segments)
        {
            const float dist = std::abs(seg.position.y - pos.y);
            if (dist < closestRowDist)
            {
                closestRowDist = dist;
                targetRowY     = seg.position.y;
            }
        }

        // ── Step 3: find closest character on the target visual row ───────────
        std::size_t bestIndex = targetLine->charStart;
        float       bestDist  = std::numeric_limits<float>::max();
        const float rowH      = targetLine->rowHeight > 0.f
                                ? targetLine->rowHeight
                                : static_cast<float>(_defaultSize);

        for (const auto& seg : targetLine->segments)
        {
            // Skip segments on a different visual row
            if (std::abs(seg.position.y - targetRowY) > rowH * 0.5f) continue;

            const std::size_t len = seg.bufEnd - seg.bufStart;
            for (std::size_t i = 0; i <= len; ++i)
            {
                const sf::Vector2f cp   = seg.sfText.findCharacterPos(i);
                const float        dist = std::abs(cp.x - pos.x);
                if (dist < bestDist)
                {
                    bestDist  = dist;
                    bestIndex = seg.bufStart + i;
                }
            }
        }
        return bestIndex;
    }

    std::size_t RichTextRenderer::lineIndexForChar(std::size_t charIndex) const
    {
        for (std::size_t i = 0; i < _lines.size(); ++i)
            if (charIndex >= _lines[i].charStart && charIndex <= _lines[i].charEnd)
                return i;
        return _lines.empty() ? 0 : _lines.size() - 1;
    }

    std::size_t RichTextRenderer::charIndexAbove(std::size_t cursorIndex) const
    {
        const std::size_t li = lineIndexForChar(cursorIndex);
        if (li == 0) return cursorIndex;
        const sf::Vector2f curPos    = charIndexToPosition(cursorIndex);
        const sf::Vector2f targetPos = {curPos.x, _lines[li-1].y + _lines[li-1].height / 2.f};
        return positionToCharIndex(targetPos);
    }

    std::size_t RichTextRenderer::charIndexBelow(std::size_t cursorIndex) const
    {
        const std::size_t li = lineIndexForChar(cursorIndex);
        if (li + 1 >= _lines.size()) return cursorIndex;
        const sf::Vector2f curPos    = charIndexToPosition(cursorIndex);
        const sf::Vector2f targetPos = {curPos.x, _lines[li+1].y + _lines[li+1].height / 2.f};
        return positionToCharIndex(targetPos);
    }

    float RichTextRenderer::getTotalHeight() const
    {
        if (_lines.empty()) return 0.f;
        const auto& last = _lines.back();
        return last.y + last.height - _origin.y;
    }

    const std::vector<RenderedLine>& RichTextRenderer::getLines() const
    {
        return _lines;
    }

} // namespace