// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#include <Malena/Graphics/Controls/EditableList.h>
#include <Malena/Utilities/Utf8.h>
#include <Malena/Graphics/Text/TextInput.h>
#include <Malena/Resources/IconFont.h>
#include <algorithm>

namespace ml
{
    // ── Row: one [selector | content | action] line, laying itself out ──────────
    // A self-contained Panel so the ScrollPane can stack/scroll it as a single
    // child while the three slots stay side-by-side and follow the scroll offset.
    struct EditableList::Row : public Panel
    {
        RectangleButton       selector;
        std::unique_ptr<Core> content;
        RectangleButton       action;
        std::function<void(sf::Vector2f)> resizeContent;   // set when content is sizeable

        bool  selected     = false;
        bool  showSelector = true;
        bool  showAction   = true;
        float slotW        = 32.f;
        float gap          = 6.f;

        void init()
        {
            setFillColor(sf::Color::Transparent);
            addUntracked(selector);
            if (content) addUntracked(*content);
            addUntracked(action);
        }

        void setPosition(const sf::Vector2f& p) override { Panel::setPosition(p); relayout(); }
        void setSize(const sf::Vector2f& s)               { Panel::setSize(s);     relayout(); }

        void relayout()
        {
            const sf::Vector2f p = getPosition();
            const float w  = getSize().x;
            const float wh = std::max(10.f, getSize().y - gap);   // widget height (gap below)

            selector.setVisible(showSelector);  selector.setEnabled(showSelector);
            action.setVisible(showAction);       action.setEnabled(showAction);

            const float sel  = showSelector ? slotW : 0.f;
            const float sgap = showSelector ? gap   : 0.f;
            const float act  = showAction   ? slotW : 0.f;
            const float agap = showAction   ? gap   : 0.f;
            const float cw   = std::max(20.f, w - sel - sgap - act - agap);

            float x = p.x;
            if (showSelector) { selector.setSize({ slotW, wh }); selector.setPosition({ x, p.y }); x += slotW + gap; }
            if (content)
            {
                content->setPosition({ x, p.y });
                if (resizeContent) resizeContent({ cw, wh });
            }
            x += cw + (showAction ? gap : 0.f);
            if (showAction) { action.setSize({ slotW, wh }); action.setPosition({ x, p.y }); }
        }
    };

    // ── Construction ────────────────────────────────────────────────────────────

    EditableList::EditableList()
    {
        setFillColor(sf::Color::Transparent);
        _scroll.setBackgroundColor(sf::Color::Transparent);

        _addBtn.setString(ml::utf8(_addLabel));
        _addBtn.setCharacterSize(13);
        _addBtn.setFillColor(sf::Color(50, 90, 160));
        _addBtn.setTextColor(sf::Color::White);
        _addBtn.onClick([this] {
            if (rowCount() < _maxRows) { addRow(); relayout(); fireChange(); }
        });

        addComponent(_scroll);   // before the add button (draw order is moot here, but
        addComponent(_addBtn);   // keeps the scroll content behind any future overlays)
    }

    EditableList::~EditableList() = default;

    // ── Configuration ─────────────────────────────────────────────────────────--

    void EditableList::setSelectionMode(SelectionMode mode)
    {
        _mode = mode;
        for (auto& r : _rows) r->showSelector = (_mode != SelectionMode::None);

        // Single-select can hold at most one — keep the first marked row.
        if (_mode == SelectionMode::Single)
        {
            bool kept = false;
            for (auto& r : _rows)
            {
                if (r->selected && !kept) kept = true;
                else                      r->selected = false;
            }
        }
        refreshSelectors();
        relayout();
    }

    void EditableList::setContentFactory(ContentFactory factory) { _contentFactory = std::move(factory); }

    void EditableList::setPlaceholder(const std::string& text)
    {
        _placeholder = text;
        for (auto& r : _rows)
            if (auto* ti = dynamic_cast<TextInput*>(r->content.get())) ti->setPlaceholder(text);
    }

    void EditableList::setShowActions(bool show)
    {
        _showActions = show;
        for (auto& r : _rows) r->showAction = show;
        relayout();
    }

    void EditableList::setShowAddButton(bool show)      { _showAdd = show; relayout(); }
    void EditableList::setAddButtonLabel(const std::string& label) { _addLabel = label; _addBtn.setString(ml::utf8(label)); }
    void EditableList::setMinRows(int n)                { _minRows = n; }
    void EditableList::setMaxRows(int n)                { _maxRows = n; }
    void EditableList::setRowHeight(float h)            { _rowHeight = h; relayout(); }

    // ── Rows ──────────────────────────────────────────────────────────────────--

    std::unique_ptr<Core> EditableList::makeContent() const
    {
        if (_contentFactory) return _contentFactory();
        auto ti = std::make_unique<TextInput>();
        ti->setPlaceholder(_placeholder);
        return ti;
    }

    int EditableList::addRow()
    {
        if (rowCount() >= _maxRows) return -1;

        auto row = std::make_unique<Row>();
        Row* r = row.get();

        r->content      = makeContent();
        r->showSelector = (_mode != SelectionMode::None);
        r->showAction   = _showActions;
        r->init();

        // Size the content to fill the slot when it's a TextInput (the default).
        if (auto* ti = dynamic_cast<TextInput*>(r->content.get()))
            r->resizeContent = [ti](sf::Vector2f s) { ti->setSize(s); };

        r->selector.onClick([this, r] { onSelectorClicked(r); });

        r->action.setFont(iconFont());
        r->action.setString(sf::String(Icon::Delete));
        r->action.setCharacterSize(18);
        r->action.setFillColor(sf::Color(180, 70, 70));
        r->action.setTextColor(sf::Color::White);
        r->action.onClick([this, r] { removeRow(indexOf(r)); fireChange(); });

        _scroll.addComponent(*r);
        _rows.push_back(std::move(row));

        refreshSelectors();
        relayout();
        return static_cast<int>(_rows.size()) - 1;
    }

    void EditableList::removeRow(int index)
    {
        if (index < 0 || index >= rowCount()) return;
        if (rowCount() <= _minRows) return;

        const bool wasSelected = _rows[index]->selected;
        _scroll.removeComponent(*_rows[index]);
        _rows.erase(_rows.begin() + index);

        // Single-select: if the answer was removed, default to the first row.
        if (_mode == SelectionMode::Single && wasSelected && !_rows.empty())
        {
            bool any = false;
            for (auto& r : _rows) any = any || r->selected;
            if (!any) _rows.front()->selected = true;
        }
        refreshSelectors();
        relayout();
    }

    void EditableList::clearRows()
    {
        for (auto& r : _rows) _scroll.removeComponent(*r);
        _rows.clear();
        relayout();
    }

    int   EditableList::rowCount() const { return static_cast<int>(_rows.size()); }
    Core* EditableList::contentAt(int index) const
    {
        if (index < 0 || index >= rowCount()) return nullptr;
        return _rows[index]->content.get();
    }

    // ── Selection ─────────────────────────────────────────────────────────────--

    void EditableList::setSelected(int index, bool selected)
    {
        if (index < 0 || index >= rowCount()) return;
        if (_mode == SelectionMode::Single && selected)
            for (auto& r : _rows) r->selected = false;
        _rows[index]->selected = selected;
        refreshSelectors();
    }

    std::vector<int> EditableList::selectedIndices() const
    {
        std::vector<int> out;
        for (int i = 0; i < rowCount(); ++i)
            if (_rows[i]->selected) out.push_back(i);
        return out;
    }

    void EditableList::onSelectorClicked(Row* row)
    {
        if (_mode == SelectionMode::None) return;
        if (_mode == SelectionMode::Single)
            for (auto& r : _rows) r->selected = (r.get() == row);
        else
            row->selected = !row->selected;
        refreshSelectors();
        fireChange();
    }

    void EditableList::refreshSelectors()
    {
        for (auto& r : _rows)
        {
            if (r->selected)
            {
                r->selector.setFont(iconFont());
                r->selector.setString(sf::String(Icon::Check));
                r->selector.setCharacterSize(18);
                r->selector.setFillColor(sf::Color(60, 160, 90));   // green = chosen
                r->selector.setTextColor(sf::Color::White);
            }
            else
            {
                r->selector.setString(sf::String());                 // empty = unselected
                r->selector.setFillColor(sf::Color(70, 75, 85));
            }
        }
    }

    // ── Text convenience ──────────────────────────────────────────────────────--

    void EditableList::setValues(const std::vector<std::string>& vals,
                                 const std::vector<int>& selected)
    {
        clearRows();
        for (const auto& v : vals)
        {
            const int i = addRow();
            if (i < 0) break;
            if (auto* ti = dynamic_cast<TextInput*>(_rows[i]->content.get())) ti->setValue(v);
        }
        for (int idx : selected) setSelected(idx, true);
        relayout();
    }

    std::vector<std::string> EditableList::values() const
    {
        std::vector<std::string> out;
        out.reserve(_rows.size());
        for (auto& r : _rows)
        {
            if (auto* ti = dynamic_cast<TextInput*>(r->content.get())) out.push_back(ti->getValue());
            else out.emplace_back();
        }
        return out;
    }

    void EditableList::onChange(std::function<void()> cb) { _onChange = std::move(cb); }
    void EditableList::fireChange() const { if (_onChange) _onChange(); }

    int EditableList::indexOf(const Row* row) const
    {
        for (int i = 0; i < rowCount(); ++i)
            if (_rows[i].get() == row) return i;
        return -1;
    }

    // ── Layout ────────────────────────────────────────────────────────────────--

    void EditableList::setPosition(const sf::Vector2f& pos) { Panel::setPosition(pos); relayout(); }
    void EditableList::setSize(const sf::Vector2f& size)    { Panel::setSize(size);     relayout(); }

    void EditableList::onEnabledChanged(bool enabled)
    {
        // Core/Panel cascade reaches the tracked children; the rows live in the
        // ScrollPane, so mirror the state onto them here. Fires on both the direct
        // and cascade paths (the old setEnabled-only override missed the latter).
        for (auto& r : _rows) r->setEnabled(enabled);
    }

    void EditableList::setVisible(bool visible)
    {
        Panel::setVisible(visible);
        _scroll.setVisible(visible);
        _addBtn.setVisible(visible && _showAdd);
        for (auto& r : _rows) r->setVisible(visible);
    }

    void EditableList::relayout()
    {
        // Geometry only — visibility/enabled are owned by setVisible/setEnabled so a
        // hidden list (parked off-screen by a host) doesn't re-show itself on resize.
        const sf::Vector2f p = getPosition();
        const sf::Vector2f s = getSize();
        const float gap  = 6.f;
        const float addH = 28.f;

        const float scrollH = std::max(0.f, s.y - (_showAdd ? addH + 10.f : 0.f));
        _scroll.setPosition(p);
        _scroll.setSize(s.x, scrollH);

        const float rowW = std::max(20.f, s.x - 10.f);   // leave room for the scrollbar
        for (auto& r : _rows) r->setSize({ rowW, _rowHeight + gap });
        _scroll.setContentHeight(0.f);                    // re-stack (also marks dirty)

        _addBtn.setSize({ 130.f, addH });
        _addBtn.setPosition({ p.x, p.y + s.y - addH });
    }

} // namespace ml
