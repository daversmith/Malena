// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

//
// DragList — a reusable drag-to-reorder + tap controller for a vertical list of
// fixed-height rows. It owns ONLY the interaction state machine; it draws nothing.
// Each frame the owner sets the list geometry + callbacks, then feeds the cursor
// and button state to update(); the owner renders the rows using displayOrder()
// (the dragged row floats to its drop slot), isDragging()/dragIndex() for the gap,
// and ghostY() for the floating row. On release it fires onReorder(from,to) after
// a real drag, or onTap(index) after a click.
//
// This replaces the hand-rolled per-hook drag bookkeeping (press edge tracking,
// threshold, drop-slot math, order permutation).
//
#ifndef MALENA_DRAGLIST_H
#define MALENA_DRAGLIST_H

#include <algorithm>
#include <cmath>
#include <functional>
#include <vector>

namespace ml {

class DragList
{
public:
    static constexpr float kDragThreshold = 6.f;   // px before a press counts as a drag

    /// Set the active list's geometry (all in the owner's local coords). `hitX0..hitX1`
    /// is the horizontal band that starts a drag (exclude per-row buttons). count==0
    /// disables dragging.
    void setGeometry(float hitX0, float hitX1, float rowTop, int count, float rowHeight)
    {
        _hitX0 = hitX0; _hitX1 = hitX1; _rowTop = rowTop; _count = count; _rowH = rowHeight;
    }
    void onReorder(std::function<void(int from, int to)> cb) { _onReorder = std::move(cb); }
    void onTap    (std::function<void(int index)>        cb) { _onTap     = std::move(cb); }

    /// Feed once per frame with the cursor in the SAME local coords as setGeometry,
    /// the left-button state, and whether the list is currently interactive.
    /// Returns true when the visual state changed (owner should re-render).
    bool update(float localX, float localY, bool down, bool live)
    {
        bool changed = false;
        if (live && _count > 0 && _onReorder)
        {
            if (down && !_prevDown)                                   // press → maybe grab a row
            {
                if (localX >= _hitX0 && localX <= _hitX1
                    && localY >= _rowTop && localY < _rowTop + _count * _rowH)
                { _dragIdx = (int)((localY - _rowTop) / _rowH); _startY = localY; _dragging = false; }
            }
            else if (down && _dragIdx >= 0)                           // held → drag
            {
                if (!_dragging && std::abs(localY - _startY) > kDragThreshold) { _dragging = true; _dropIdx = _dragIdx; changed = true; }
                if (_dragging)
                {
                    _mouseY = localY;
                    _dropIdx = std::max(0, std::min((int)((localY - _rowTop + _rowH * 0.5f) / _rowH), _count - 1));
                    changed = true;                                  // ghost tracks the cursor
                }
            }
            else if (!down && _prevDown && _dragIdx >= 0)            // release → reorder or tap
            {
                if (_dragging) { if (_dropIdx != _dragIdx && _dropIdx >= 0) _onReorder(_dragIdx, _dropIdx); }
                else if (_onTap) _onTap(_dragIdx);
                _dragIdx = _dropIdx = -1; _dragging = false; changed = true;
            }
        }
        else if (!down) { _dragIdx = _dropIdx = -1; _dragging = false; }

        _prevDown = down;
        return changed;
    }

    /// Row indices in display order: the dragged row sits at its drop slot; identity
    /// order otherwise. The owner draws a gap where `slot`'s index == dragIndex().
    std::vector<int> displayOrder(int count) const
    {
        std::vector<int> order(count);
        for (int i = 0; i < count; ++i) order[i] = i;
        if (_dragging && _dragIdx >= 0 && _dragIdx < count
            && _dropIdx >= 0 && _dropIdx < count && _dropIdx != _dragIdx)
        {
            order.erase(order.begin() + _dragIdx);
            order.insert(order.begin() + _dropIdx, _dragIdx);
        }
        return order;
    }

    bool isDragging() const { return _dragging; }
    int  dragIndex()  const { return _dragIdx; }
    int  dropIndex()  const { return _dropIdx; }

    /// Clamped Y for the floating ghost row (owner draws the dragged row here).
    float ghostY() const
    {
        const float gy = _mouseY - _rowH * 0.45f;
        return std::max(_rowTop - 6.f, std::min(gy, _rowTop + (_count - 1) * _rowH + 6.f));
    }

private:
    float _hitX0 = 0.f, _hitX1 = 0.f, _rowTop = 0.f, _rowH = 44.f;
    int   _count = 0;
    int   _dragIdx = -1, _dropIdx = -1;
    bool  _dragging = false, _prevDown = false;
    float _startY = 0.f, _mouseY = 0.f;
    std::function<void(int, int)> _onReorder;
    std::function<void(int)>      _onTap;
};

} // namespace ml

#endif // MALENA_DRAGLIST_H
