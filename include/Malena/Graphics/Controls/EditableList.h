// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#ifndef MALENA_EDITABLELIST_H
#define MALENA_EDITABLELIST_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Core.h>
#include <Malena/Graphics/Layouts/Panel.h>
#include <Malena/Graphics/Controls/ScrollPane.h>
#include <Malena/Graphics/Controls/RectangleButton.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace ml
{
    /**
     * @brief A scrollable, editable list of rows — each row is three configurable
     *        slots: a @b selector, the @b content, and an @b action.
     * @ingroup GraphicsControls
     *
     * @verbatim
     *   [ selector ]   [ content ........... ]   [ action ]
     *     radio /         editable TextInput        delete
     *     checkbox /      (or any component via      (or none /
     *     none             a content factory)         custom)
     * @endverbatim
     *
     * Combines two classic patterns — a @e repeater (add/remove editable rows) and
     * a @e single/multi-select list (one or many rows marked) — over the
     * viewport-clipping @c ScrollPane. An "+ Add" button (optional) appends rows.
     *
     * The @b content slot defaults to a @c TextInput (so the common "list of text
     * options" case needs no configuration); pass @c setContentFactory to put any
     * component in each row. The @b selector slot is driven by @c SelectionMode;
     * the @b action slot is a per-row delete button (toggle with @c setShowActions).
     *
     * ### MC-options example
     * @code
     * ml::EditableList opts;
     * opts.setSelectionMode(ml::EditableList::SelectionMode::Single);
     * opts.setPlaceholder("Option text");
     * opts.setValues({"Red", "Green", "Blue"}, { 1 });   // Green is the answer
     * // ... later:
     * auto texts = opts.values();              // {"Red","Green","Blue"}
     * auto right = opts.selectedIndices();     // { 1 }
     * @endcode
     *
     * @see ScrollPane, List, RadioGroup, TextInput
     */
    class MALENA_API EditableList : public Panel
    {
    public:
        enum class SelectionMode { None, Single, Multi };

        /// Builds the @b content component for a new row. Ownership transfers to
        /// the list. Defaults to a @c TextInput when unset.
        using ContentFactory = std::function<std::unique_ptr<Core>()>;

        EditableList();
        ~EditableList() override;   // out-of-line for the pimpl-style Row

        // Non-copyable. Required, not stylistic: __declspec(dllexport) on a class
        // instantiates EVERY member, including the implicit copy assignment — and
        // that cannot be formed for a std::vector<std::unique_ptr<...>> member, so
        // MSVC fails the shared/DLL build with C2280. (Static builds and Clang/GCC
        // never instantiate it, which is why this only broke Windows Shared.)
        // Copying a scene-graph node was never valid anyway.
        EditableList(const EditableList&)            = delete;
        EditableList& operator=(const EditableList&) = delete;

        // ── Configuration ──────────────────────────────────────────────────────
        void setSelectionMode(SelectionMode mode);
        void setContentFactory(ContentFactory factory);
        void setPlaceholder(const std::string& text);   // default TextInput content
        void setShowActions(bool show);                 // per-row delete button
        void setShowAddButton(bool show);
        void setAddButtonLabel(const std::string& label);
        void setMinRows(int n);
        void setMaxRows(int n);
        void setRowHeight(float h);

        // ── Rows ────────────────────────────────────────────────────────────────
        int  addRow();                                  // append; returns its index
        void removeRow(int index);
        void clearRows();
        [[nodiscard]] int   rowCount() const;
        [[nodiscard]] Core* contentAt(int index) const; // the row's content widget

        // ── Selection ─────────────────────────────────────────────────────────--
        void setSelected(int index, bool selected);
        [[nodiscard]] std::vector<int> selectedIndices() const;

        // ── Text convenience (default TextInput content) ─────────────────────────
        /// Replace all rows with one per value; @p selected marks the chosen rows.
        void setValues(const std::vector<std::string>& values,
                       const std::vector<int>& selected = {});
        /// Text of every row, in order (includes empty rows — caller filters).
        [[nodiscard]] std::vector<std::string> values() const;

        // ── Callbacks ─────────────────────────────────────────────────────────--
        void onChange(std::function<void()> cb);

        // ── Layout ────────────────────────────────────────────────────────────--
        void setPosition(const sf::Vector2f& pos) override;
        void setSize(const sf::Vector2f& size);
        void setVisible(bool visible) override;

    protected:
        // Rows live in the ScrollPane (not this Panel's child list), so mirror the
        // enabled state onto them here — fires on both direct and cascade paths.
        void onEnabledChanged(bool enabled) override;

    private:
        struct Row;                                      // defined in the .cpp

        std::unique_ptr<Core> makeContent() const;       // factory or default TextInput
        void   relayout();
        void   onSelectorClicked(Row* row);
        void   refreshSelectors();
        int    indexOf(const Row* row) const;
        void   fireChange() const;

        ScrollPane                        _scroll{ 10.f, 10.f };
        RectangleButton                   _addBtn;
        std::vector<std::unique_ptr<Row>> _rows;

        SelectionMode  _mode          = SelectionMode::Single;
        ContentFactory _contentFactory;
        std::string    _placeholder;
        std::string    _addLabel      = "+ Add";
        bool           _showActions   = true;
        bool           _showAdd       = true;
        int            _minRows        = 0;
        int            _maxRows        = 100;
        float          _rowHeight      = 34.f;

        std::function<void()> _onChange;
    };

} // namespace ml

#endif // MALENA_EDITABLELIST_H
