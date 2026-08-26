// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0

#include <Malena/Graphics/Controls/ThumbnailPicker.h>
#include <Malena/Utilities/Utf8.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cmath>

namespace ml
{
    ThumbnailPicker::ThumbnailPicker(const sf::Font& font)
        : _font(&font)
    {
        // Hit-test clicks ourselves (mirrors Modal) so no child components are
        // needed — avoids origin-parked controls grabbing input.
        onUpdate([this]
        {
            const bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            if (_active && down && !_prevDown)
            {
                const auto& win = WindowManager::getWindow();
                const sf::Vector2f wp =
                    win.mapPixelToCoords(sf::Mouse::getPosition(win));
                for (int i = 0; i < static_cast<int>(_cells.size()); ++i)
                    if (_cells[i].contains(wp)) { setSelectedIndex(i); break; }
            }
            _prevDown = down;
        });
    }

    // ── Layout ──────────────────────────────────────────────────────────────────

    void ThumbnailPicker::layout()
    {
        _cells.clear();
        const int n = static_cast<int>(_textures.size());
        for (int i = 0; i < n; ++i)
        {
            const int c = i % _columns;
            const int r = i / _columns;
            _cells.push_back({
                {_position.x + c * (_tileW + _gap),
                 _position.y + r * (_tileH + _gap)},
                {_tileW, _tileH}});
        }
    }

    // ── Content ─────────────────────────────────────────────────────────────────

    void ThumbnailPicker::clear()
    {
        _textures.clear();
        _captions.clear();
        _cells.clear();
        _selected = 0;
    }

    void ThumbnailPicker::addThumbnail(const sf::Image& image, const std::string& caption)
    {
        auto tex = std::make_unique<sf::Texture>();
        if (tex->resize(image.getSize()))
        {
            tex->update(image);
            tex->setSmooth(true);
        }
        _textures.push_back(std::move(tex));
        _captions.push_back(caption);
        layout();
    }

    int ThumbnailPicker::count() const { return static_cast<int>(_textures.size()); }

    void ThumbnailPicker::setColumns(int columns)
    {
        _columns = std::max(1, columns);
        layout();
    }

    void ThumbnailPicker::setTileSize(float w, float h)
    {
        _tileW = w; _tileH = h;
        layout();
    }

    // ── Selection ───────────────────────────────────────────────────────────────

    void ThumbnailPicker::setSelectedIndex(int index)
    {
        if (index < 0 || index >= count()) return;
        if (index == _selected && !_cells.empty()) { /* still notify on re-pick? no */ }
        _selected = index;
        if (_onSelectionChanged) _onSelectionChanged(_selected);
    }

    int  ThumbnailPicker::getSelectedIndex() const { return _selected; }

    void ThumbnailPicker::onSelectionChanged(std::function<void(int)> cb)
    { _onSelectionChanged = std::move(cb); }

    void ThumbnailPicker::setActive(bool active)
    {
        _active = active;
        // Swallow the click that opened us so it isn't read as a tile pick.
        _prevDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    }

    // ── Draw ────────────────────────────────────────────────────────────────────

    void ThumbnailPicker::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        const float imgH = _tileH - _captionH;

        for (int i = 0; i < static_cast<int>(_cells.size()); ++i)
        {
            const sf::FloatRect& cell = _cells[i];
            const bool sel = (i == _selected);

            // Thumbnail backing (letterbox) + selection frame.
            sf::RectangleShape bg({_tileW, imgH});
            bg.setPosition(cell.position);
            bg.setFillColor(sf::Color(20, 22, 28));
            bg.setOutlineThickness(sel ? 3.f : 1.f);
            bg.setOutlineColor(sel ? sf::Color(90, 150, 240) : sf::Color(70, 76, 88));
            target.draw(bg, states);

            // Aspect-fit the captured image into the tile.
            const sf::Texture& tex = *_textures[i];
            const sf::Vector2u ts = tex.getSize();
            if (ts.x > 0 && ts.y > 0)
            {
                const float scale = std::min((_tileW - 6.f) / ts.x, (imgH - 6.f) / ts.y);
                sf::Sprite spr(tex);
                spr.setScale({scale, scale});
                spr.setPosition({
                    cell.position.x + (_tileW - ts.x * scale) * 0.5f,
                    cell.position.y + (imgH  - ts.y * scale) * 0.5f});
                target.draw(spr, states);
            }

            // Caption, centred and clipped to the tile width.
            sf::Text label(*_font, ml::utf8(_captions[i]), 12);
            std::string s = _captions[i];
            while (!s.empty() && label.getLocalBounds().size.x > _tileW - 8.f)
            {
                s.pop_back();
                label.setString(ml::utf8(s + "..."));
            }
            label.setFillColor(sel ? sf::Color(210, 225, 250) : sf::Color(170, 176, 188));
            const auto lb = label.getLocalBounds();
            label.setPosition({
                cell.position.x + (_tileW - lb.size.x) * 0.5f,
                cell.position.y + imgH + 4.f});
            target.draw(label, states);
        }
    }

    // ── Positionable ──────────────────────────────────────────────────────────

    void ThumbnailPicker::setPosition(const sf::Vector2f& position)
    { _position = position; layout(); }

    sf::Vector2f ThumbnailPicker::getPosition() const { return _position; }

    sf::FloatRect ThumbnailPicker::getGlobalBounds() const
    {
        const int n = count();
        if (n == 0) return sf::FloatRect{_position, {0.f, 0.f}};
        const int cols = std::min(_columns, n);
        const int rows = (n + _columns - 1) / _columns;
        return sf::FloatRect{_position, {
            cols * _tileW + (cols - 1) * _gap,
            rows * _tileH + (rows - 1) * _gap}};
    }

} // namespace ml
