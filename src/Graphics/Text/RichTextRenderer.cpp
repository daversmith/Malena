// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Text/RichTextRenderer.h>
#include <algorithm>
#include <cmath>
#include <limits>

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
        layoutLines();
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
            float x = _origin.x;

            for (auto& seg : line.segments)
            {
                seg.position = {x, y};
                seg.sfText.setPosition({x, y});

                const float segWidth = seg.sfText.getGlobalBounds().size.x;

                if (_maxWidth > 0.f && x + segWidth - _origin.x > _maxWidth && x > _origin.x)
                {
                    y += line.height;
                    x  = _origin.x;
                    seg.position = {x, y};
                    seg.sfText.setPosition({x, y});
                }

                x += seg.sfText.getGlobalBounds().size.x;
            }

            y += line.height;
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

            float lineHeight = static_cast<float>(_defaultSize);
            for (const auto& seg : line.segments)
                lineHeight = std::max(lineHeight,
                    static_cast<float>(seg.sfText.getCharacterSize()));
            line.height = lineHeight;

            float x = _origin.x;
            for (auto& seg : line.segments)
            {
                seg.position = {x, y};
                seg.sfText.setPosition({x, y});

                const float segWidth = seg.sfText.getGlobalBounds().size.x;

                if (_maxWidth > 0.f && x + segWidth - _origin.x > _maxWidth && x > _origin.x)
                {
                    y += lineHeight;
                    x  = _origin.x;
                    seg.position = {x, y};
                    seg.sfText.setPosition({x, y});
                }

                x += seg.sfText.getGlobalBounds().size.x;
            }

            y += lineHeight;
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
            for (const auto& seg : line.segments)
                target.draw(seg.sfText, states);
    }

    void RichTextRenderer::drawSelection(sf::RenderTarget&       target,
                                         const sf::RenderStates& states,
                                         std::size_t             selStart,
                                         std::size_t             selEnd,
                                         const sf::Color&        color) const
    {
        if (selStart >= selEnd) return;

        const sf::Vector2f startPos = charIndexToPosition(selStart);
        const sf::Vector2f endPos   = charIndexToPosition(selEnd);

        const std::size_t startLine = lineIndexForChar(selStart);
        const std::size_t endLine   = lineIndexForChar(selEnd);

        if (startLine == endLine)
        {
            sf::RectangleShape rect;
            rect.setPosition({startPos.x, _lines[startLine].y});
            rect.setSize({endPos.x - startPos.x, _lines[startLine].height});
            rect.setFillColor(color);
            target.draw(rect, states);
        }
        else
        {
            const float lineRight = _origin.x + (_maxWidth > 0.f ? _maxWidth : 10000.f);

            {
                sf::RectangleShape rect;
                const auto& line = _lines[startLine];
                rect.setPosition({startPos.x, line.y});
                rect.setSize({lineRight - startPos.x, line.height});
                rect.setFillColor(color);
                target.draw(rect, states);
            }

            for (std::size_t l = startLine + 1; l < endLine; ++l)
            {
                sf::RectangleShape rect;
                const auto& line = _lines[l];
                rect.setPosition({_origin.x, line.y});
                rect.setSize({lineRight - _origin.x, line.height});
                rect.setFillColor(color);
                target.draw(rect, states);
            }

            {
                sf::RectangleShape rect;
                const auto& line = _lines[endLine];
                rect.setPosition({_origin.x, line.y});
                rect.setSize({endPos.x - _origin.x, line.height});
                rect.setFillColor(color);
                target.draw(rect, states);
            }
        }
    }

    void RichTextRenderer::drawCursor(sf::RenderTarget&       target,
                                      const sf::RenderStates& states,
                                      std::size_t             charIndex,
                                      const sf::Color&        color,
                                      float                   width) const
    {
        const sf::Vector2f pos = charIndexToPosition(charIndex);
        const std::size_t  li  = lineIndexForChar(charIndex);
        const float        h   = _lines.empty()
                                 ? static_cast<float>(_defaultSize)
                                 : _lines[li].height;

        sf::RectangleShape cursor({width, h});
        cursor.setPosition(pos);
        cursor.setFillColor(color);
        target.draw(cursor, states);
    }

    // ── Coordinate mapping ────────────────────────────────────────────────────

    sf::Vector2f RichTextRenderer::charIndexToPosition(std::size_t index) const
    {
        if (_lines.empty()) return _origin;

        for (const auto& line : _lines)
        {
            if (index < line.charStart || index > line.charEnd) continue;

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
            return {_origin.x, line.y};
        }

        const auto& lastLine = _lines.back();
        if (!lastLine.segments.empty())
        {
            const auto& lastSeg = lastLine.segments.back();
            return lastSeg.sfText.findCharacterPos(lastSeg.sfText.getString().getSize());
        }
        return {_origin.x, lastLine.y};
    }

    std::size_t RichTextRenderer::positionToCharIndex(const sf::Vector2f& pos) const
    {
        if (_lines.empty()) return 0;

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

        std::size_t bestIndex = targetLine->charStart;
        float       bestDist  = std::numeric_limits<float>::max();

        for (const auto& seg : targetLine->segments)
        {
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