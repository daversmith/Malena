// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Graphics/Controls/SplitPanel.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace ml
{
    SplitPanel::SplitPanel(Orientation orientation_, const sf::Font& font_)
    {
        SplitPanelTheme::applyFrom(ThemeManager::get());
        this->font        = &font_;
        this->orientation = orientation_;

        // ── Per-frame: hover + drag ───────────────────────────────────────────
        onUpdate([this]{
            const sf::Vector2f wp = WindowManager::getWindow().mapPixelToCoords(
                sf::Mouse::getPosition(WindowManager::getWindow()));
            const bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

            // Press start
            if (mouseDown && !_prevMouseDown)
            {
                const int hit = hitTestDivider(wp);
                if (hit >= 0 && resizable)
                {
                    _draggingDiv    = hit;
                    _dragStartMouse = (orientation == Orientation::HORIZONTAL)
                                      ? wp.x : wp.y;
                    _dragStartSizeA = _panes[hit].size;
                    _dragStartSizeB = _panes[hit + 1].size;
                    setState(State::DIVIDER_DRAG);
                }
            }
            // Dragging
            else if (mouseDown && _draggingDiv >= 0)
            {
                const float pos   = (orientation == Orientation::HORIZONTAL)
                                    ? wp.x : wp.y;
                const float delta = pos - _dragStartMouse;

                const float minA = std::max(_panes[_draggingDiv].minSize,
                                            minPaneSize);
                const float minB = std::max(_panes[_draggingDiv+1].minSize,
                                            minPaneSize);
                const float maxA = _panes[_draggingDiv].maxSize > 0.f
                                   ? _panes[_draggingDiv].maxSize
                                   : _dragStartSizeA + _dragStartSizeB - minB;
                const float maxB = _panes[_draggingDiv+1].maxSize > 0.f
                                   ? _panes[_draggingDiv+1].maxSize
                                   : _dragStartSizeA + _dragStartSizeB - minA;

                float newA = std::clamp(_dragStartSizeA + delta, minA, maxA);
                float newB = std::clamp(
                    _dragStartSizeA + _dragStartSizeB - newA, minB, maxB);
                newA = _dragStartSizeA + _dragStartSizeB - newB;

                _panes[_draggingDiv].size     = newA;
                _panes[_draggingDiv + 1].size = newB;
                layoutPanes();

                if (_onDividerMoved)
                    _onDividerMoved(static_cast<std::size_t>(_draggingDiv),
                                    dividerPos(_draggingDiv));
            }
            // Release
            else if (!mouseDown && _draggingDiv >= 0)
            {
                _draggingDiv = -1;
                setState(State::IDLE);
            }

            // Hover (no drag)
            if (_draggingDiv < 0)
            {
                _hoveredDivider = hitTestDivider(wp);
                setState(_hoveredDivider >= 0 ? State::DIVIDER_HOVER : State::IDLE);
            }

            _prevMouseDown = mouseDown;
        });
    }

    // ── onThemeApplied ────────────────────────────────────────────────────────

    void SplitPanel::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        SplitPanelTheme::applyFrom(theme);
    }

    // ── Geometry ─────────────────────────────────────────────────────────────

    float SplitPanel::totalAvailable() const
    {
        const float total = (orientation == Orientation::HORIZONTAL)
                            ? _size.x : _size.y;
        const float dividers = static_cast<float>(
            _panes.empty() ? 0 : _panes.size() - 1) * dividerThick;
        return total - dividers;
    }

    float SplitPanel::dividerPos(int divIdx) const
    {
        // Returns position along the split axis
        float pos = (orientation == Orientation::HORIZONTAL)
                    ? _position.x : _position.y;
        for (int i = 0; i <= divIdx; ++i)
        {
            pos += _panes[i].size;
            if (i < divIdx) pos += dividerThick;
        }
        return pos;
    }

    int SplitPanel::hitTestDivider(const sf::Vector2f& wp) const
    {
        if (_panes.size() < 2) return -1;
        const float grab = std::max(dividerThick, 6.f); // at least 6px grab area

        for (int i = 0; i < static_cast<int>(_panes.size()) - 1; ++i)
        {
            const float mid = dividerPos(i);
            sf::FloatRect divBounds;
            if (orientation == Orientation::HORIZONTAL)
                divBounds = {{mid - grab/2.f, _position.y}, {grab, _size.y}};
            else
                divBounds = {{_position.x, mid - grab/2.f}, {_size.x, grab}};

            if (divBounds.contains(wp)) return i;
        }
        return -1;
    }

    // ── layoutPanes ──────────────────────────────────────────────────────────

    void SplitPanel::layoutPanes()
    {
        if (_panes.empty()) return;

        const bool horiz = (orientation == Orientation::HORIZONTAL);
        float offset = horiz ? _position.x : _position.y;

        for (auto& pane : _panes)
        {
            sf::Vector2f pos, sz;
            if (horiz)
            {
                pos = {offset, _position.y};
                sz  = {pane.size, _size.y};
            }
            else
            {
                pos = {_position.x, offset};
                sz  = {_size.x, pane.size};
            }

            if (pane.content)
            {
                pane.content->setPosition(pos);
                // Resize if component supports setSize (duck-typed via cast)
                // We'll try a dynamic_cast to a known resizable base if available.
                // For now components resize themselves when repositioned via their layout.
            }

            offset += pane.size + dividerThick;
        }
    }

    void SplitPanel::distributeSizes()
    {
        if (_panes.empty()) return;

        const float avail = totalAvailable();

        // Count panes with no initial size set
        float fixed = 0.f;
        int   flex  = 0;
        for (const auto& p : _panes)
        {
            if (p.size > 0.f) fixed += p.size;
            else              ++flex;
        }

        const float flexSize = (flex > 0)
                               ? std::max(0.f, (avail - fixed) / static_cast<float>(flex))
                               : 0.f;

        for (auto& p : _panes)
            if (p.size <= 0.f) p.size = flexSize;

        // Final clamp to available space
        float total = 0.f;
        for (const auto& p : _panes) total += p.size;

        if (total > avail && total > 0.f)
        {
            const float scale = avail / total;
            for (auto& p : _panes) p.size *= scale;
        }
    }

    // ── draw ──────────────────────────────────────────────────────────────────

    void SplitPanel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Pane backgrounds
        const bool horiz = (orientation == Orientation::HORIZONTAL);
        float offset = horiz ? _position.x : _position.y;

        for (int i = 0; i < static_cast<int>(_panes.size()); ++i)
        {
            const auto& pane = _panes[i];

            sf::Vector2f pos = horiz
                ? sf::Vector2f{offset, _position.y}
                : sf::Vector2f{_position.x, offset};
            sf::Vector2f sz = horiz
                ? sf::Vector2f{pane.size, _size.y}
                : sf::Vector2f{_size.x, pane.size};

            if (paneBg.a > 0)
            {
                sf::RectangleShape bg(sz);
                bg.setPosition(pos);
                bg.setFillColor(paneBg);
                target.draw(bg, states);
            }

            // Content
            if (pane.content)
                target.draw(*dynamic_cast<const sf::Drawable*>(pane.content), states);

            offset += pane.size;

            // Divider (before next pane)
            if (i < static_cast<int>(_panes.size()) - 1)
            {
                const bool hov  = (i == _hoveredDivider);
                const bool drag = (i == _draggingDiv);
                drawDivider(target, states, i, hov, drag);
                offset += dividerThick;
            }
        }
    }

    void SplitPanel::drawDivider(sf::RenderTarget& target,
                                   const sf::RenderStates& states,
                                   int divIdx, bool hovered, bool dragging) const
    {
        const float pos  = dividerPos(divIdx);
        const bool horiz = (orientation == Orientation::HORIZONTAL);

        sf::Color color = dragging ? dividerDrag
                        : hovered  ? dividerHover
                        :            dividerColor;

        sf::RectangleShape div;
        if (horiz)
        {
            div.setSize({dividerThick, _size.y});
            div.setPosition({pos, _position.y});
        }
        else
        {
            div.setSize({_size.x, dividerThick});
            div.setPosition({_position.x, pos});
        }
        div.setFillColor(color);
        target.draw(div, states);

        // Grip dots for GRIP style
        if (handleStyle == HandleStyle::GRIP)
        {
            const float dotR  = 2.f;
            const float dotGap = 5.f;
            const int   dots  = 3;

            const float centerX = horiz ? pos + dividerThick / 2.f
                                        : _position.x + _size.x / 2.f;
            const float centerY = horiz ? _position.y + _size.y / 2.f
                                        : pos + dividerThick / 2.f;

            const float totalLen = static_cast<float>(dots - 1) * dotGap;

            for (int d = 0; d < dots; ++d)
            {
                const float t = static_cast<float>(d) * dotGap - totalLen / 2.f;
                sf::RectangleShape dot({dotR * 2.f, dotR * 2.f});
                dot.setFillColor(gripColor);
                if (horiz)
                    dot.setPosition({centerX - dotR, centerY + t - dotR});
                else
                    dot.setPosition({centerX + t - dotR, centerY - dotR});
                target.draw(dot, states);
            }
        }
    }

    // ── Pane management ───────────────────────────────────────────────────────

    void SplitPanel::addPane(ml::Core& content, float initialSize)
    {
        Pane pane;
        pane.content = &content;
        pane.size    = initialSize;
        _panes.push_back(std::move(pane));
        distributeSizes();
        layoutPanes();
    }

    void SplitPanel::setPaneMinSize(std::size_t index, float minSize)
    {
        if (index < _panes.size()) _panes[index].minSize = minSize;
    }

    void SplitPanel::setPaneMaxSize(std::size_t index, float maxSize)
    {
        if (index < _panes.size()) _panes[index].maxSize = maxSize;
    }

    void SplitPanel::setDividerPosition(std::size_t divIdx, float position)
    {
        if (divIdx + 1 >= _panes.size()) return;

        const float origin = (orientation == Orientation::HORIZONTAL)
                             ? _position.x : _position.y;
        // Compute where this divider starts
        float offset = origin;
        for (std::size_t i = 0; i < divIdx; ++i)
            offset += _panes[i].size + dividerThick;

        const float minA = std::max(_panes[divIdx].minSize, minPaneSize);
        const float minB = std::max(_panes[divIdx+1].minSize, minPaneSize);
        const float combined = _panes[divIdx].size + _panes[divIdx+1].size;

        float newA = std::clamp(position - offset, minA, combined - minB);
        _panes[divIdx].size     = newA;
        _panes[divIdx+1].size   = combined - newA;
        layoutPanes();

        if (_onDividerMoved)
            _onDividerMoved(divIdx, dividerPos(static_cast<int>(divIdx)));
    }

    void SplitPanel::onDividerMoved(std::function<void(std::size_t, float)> cb)
    { _onDividerMoved = std::move(cb); }

    // ── Size / position ───────────────────────────────────────────────────────

    void SplitPanel::setSize(const sf::Vector2f& sz)
    {
        _size = sz;
        distributeSizes();
        layoutPanes();
    }

    void SplitPanel::setPosition(const sf::Vector2f& pos)
    {
        _position = pos;
        layoutPanes();
    }

    sf::Vector2f  SplitPanel::getPosition()     const { return _position; }
    sf::FloatRect SplitPanel::getGlobalBounds() const
    { return sf::FloatRect{_position, _size}; }

} // namespace ml
