// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0

#include <Malena/Graphics/Controls/Table.h>
#include <Malena/Utilities/Utf8.h>
#include <Malena/Resources/ThemeManager.h>
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace ml
{

Table::Table(const sf::Font& font)
    : _font(&font)
{
    onThemeApplied(ThemeManager::get());
}

// ── onThemeApplied ────────────────────────────────────────────────────────────

void Table::onThemeApplied(const Theme& t)
{
    if (isThemeLocked()) return;
    _headerBgColor  = sf::Color(t.surface.r,     t.surface.g,     t.surface.b + 15);
    _headerTxtColor = t.onSurface;
    _rowBgColor     = t.background;
    _altRowBgColor  = sf::Color(t.background.r + 8, t.background.g + 8, t.background.b + 12);
    _textColor      = t.onBackground;
    _dividerColor   = t.border;
    _borderColor    = t.border;
    if (t.font) _font = t.font;
}

// ── Column / row management ───────────────────────────────────────────────────

void Table::addColumn(const std::string& header, float width)
{
    _columns.push_back({header, width});
}

void Table::addRow(const std::vector<std::string>& cells)
{
    if (_maxRows > 0 && _rows.size() >= _maxRows)
    {
        _rows.erase(_rows.begin());
        // Shift all row-colour overrides down by one (oldest row dropped).
        std::unordered_map<std::size_t, sf::Color> shifted;
        for (auto& [idx, c] : _rowColors)
            if (idx > 0) shifted[idx - 1] = c;
        _rowColors = std::move(shifted);
    }

    Row row(_columns.size());
    for (std::size_t i = 0; i < cells.size() && i < _columns.size(); ++i)
        row[i] = cells[i];
    _rows.push_back(std::move(row));
}

bool Table::removeRow(std::size_t index)
{
    if (index >= _rows.size()) return false;
    _rows.erase(_rows.begin() + index);
    // Shift colour overrides: discard removed row's entry, decrement all above it.
    std::unordered_map<std::size_t, sf::Color> shifted;
    for (auto& [idx, c] : _rowColors)
    {
        if (idx == index) continue;
        shifted[idx > index ? idx - 1 : idx] = c;
    }
    _rowColors = std::move(shifted);
    return true;
}

void Table::clear()
{
    _rows.clear();
    _rowColors.clear();
}

void Table::setRowColor(std::size_t rowIndex, sf::Color color) { _rowColors[rowIndex] = color; }
void Table::clearRowColor(std::size_t rowIndex)                { _rowColors.erase(rowIndex); }

void Table::reset()
{
    _rows.clear();
    _columns.clear();
}

void Table::setMaxRows(std::size_t max)
{
    _maxRows = max;
    if (_maxRows > 0)
        while (_rows.size() > _maxRows)
            _rows.erase(_rows.begin());
}

// ── Position ──────────────────────────────────────────────────────────────────

void Table::setPosition(const sf::Vector2f& pos)
{
    _position = pos;
}

sf::Vector2f  Table::getPosition()     const { return _position; }
sf::FloatRect Table::getGlobalBounds() const { return {{_position}, {_size}}; }

// ── Helpers ───────────────────────────────────────────────────────────────────

std::vector<float> Table::resolveWidths() const
{
    float totalFixed = 0.f;
    int   flexCount  = 0;
    for (const auto& col : _columns)
    {
        if (col.width > 0.f) totalFixed += col.width;
        else                 ++flexCount;
    }
    float remaining = std::max(0.f, _size.x - totalFixed);
    float flexWidth = (flexCount > 0) ? remaining / static_cast<float>(flexCount) : 0.f;

    std::vector<float> out;
    out.reserve(_columns.size());
    for (const auto& col : _columns)
        out.push_back(col.width > 0.f ? col.width : flexWidth);
    return out;
}

std::string Table::clampText(const sf::Font& font, unsigned charSize,
                              const std::string& text, float maxWidth)
{
    sf::Text t(font, ml::utf8(text), charSize);
    if (t.getLocalBounds().size.x <= maxWidth) return text;

    std::string s = text;
    while (!s.empty())
    {
        s.pop_back();
        sf::Text probe(font, ml::utf8(s + "..."), charSize);
        if (probe.getLocalBounds().size.x <= maxWidth)
            return s + "...";
    }
    return "";
}

// ── Drawing ───────────────────────────────────────────────────────────────────

void Table::drawHeader(sf::RenderTarget& target, const sf::RenderStates& states,
                        const std::vector<float>& widths) const
{
    sf::RectangleShape bg({_size.x, _headerHeight});
    bg.setPosition(_position);
    bg.setFillColor(_headerBgColor);
    target.draw(bg, states);

    float x = _position.x;
    for (std::size_t i = 0; i < _columns.size(); ++i)
    {
        const float colW = widths[i];
        const float maxTextW = colW - _padding * 2.f;

        sf::Text label(*_font,
                        ml::utf8(clampText(*_font, _headerFontSize, _columns[i].header, maxTextW)),
                        _headerFontSize);
        label.setFillColor(_headerTxtColor);

        const float textH  = label.getLocalBounds().size.y;
        const float textY  = _position.y + (_headerHeight - textH) / 2.f - label.getLocalBounds().position.y;
        label.setPosition({x + _padding, textY});
        target.draw(label, states);

        // Vertical divider (skip after last column)
        if (i + 1 < _columns.size())
        {
            sf::RectangleShape div({1.f, _headerHeight});
            div.setPosition({x + colW, _position.y});
            div.setFillColor(_dividerColor);
            target.draw(div, states);
        }
        x += colW;
    }

    // Bottom border under header
    sf::RectangleShape hline({_size.x, 1.f});
    hline.setPosition({_position.x, _position.y + _headerHeight - 1.f});
    hline.setFillColor(_dividerColor);
    target.draw(hline, states);
}

void Table::drawDataRow(sf::RenderTarget& target, const sf::RenderStates& states,
                         const Row& row, float y, bool alt,
                         const std::vector<float>& widths,
                         const sf::Color* bgOverride) const
{
    sf::RectangleShape bg({_size.x, _rowHeight});
    bg.setPosition({_position.x, y});
    bg.setFillColor(bgOverride ? *bgOverride : (alt ? _altRowBgColor : _rowBgColor));
    target.draw(bg, states);

    float x = _position.x;
    for (std::size_t i = 0; i < _columns.size(); ++i)
    {
        const float colW = widths[i];
        const float maxTextW = colW - _padding * 2.f;

        const std::string& cell = (i < row.size()) ? row[i] : "";
        if (!cell.empty())
        {
            sf::Text label(*_font,
                            ml::utf8(clampText(*_font, _fontSize, cell, maxTextW)),
                            _fontSize);
            label.setFillColor(_textColor);

            const float textH = label.getLocalBounds().size.y;
            const float textY = y + (_rowHeight - textH) / 2.f - label.getLocalBounds().position.y;
            label.setPosition({x + _padding, textY});
            target.draw(label, states);
        }

        if (i + 1 < _columns.size())
        {
            sf::RectangleShape div({1.f, _rowHeight});
            div.setPosition({x + colW, y});
            div.setFillColor(_dividerColor);
            target.draw(div, states);
        }
        x += colW;
    }
}

void Table::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (_columns.empty()) return;

    const std::vector<float> widths = resolveWidths();

    // Outer border
    sf::RectangleShape border(_size);
    border.setPosition(_position);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(_borderColor);
    border.setOutlineThickness(1.f);
    target.draw(border, states);

    drawHeader(target, states, widths);

    // Visible data rows — tail view: show newest rows that fit
    const float available = _size.y - _headerHeight;
    if (available <= 0.f || _rows.empty()) return;

    const std::size_t visibleCount =
        static_cast<std::size_t>(std::floor(available / _rowHeight));
    const std::size_t startRow =
        (_rows.size() > visibleCount) ? _rows.size() - visibleCount : 0;

    const float bottomClip = _position.y + _size.y;

    for (std::size_t i = startRow; i < _rows.size(); ++i)
    {
        const std::size_t visualIdx = i - startRow;
        const float y = _position.y + _headerHeight + static_cast<float>(visualIdx) * _rowHeight;
        if (y + _rowHeight > bottomClip) break;
        const auto it = _rowColors.find(i);
        const sf::Color* override = (it != _rowColors.end()) ? &it->second : nullptr;
        drawDataRow(target, states, _rows[i], y, (visualIdx % 2 == 1), widths, override);
    }
}

} // namespace ml
