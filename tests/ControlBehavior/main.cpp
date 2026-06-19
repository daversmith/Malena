// Control value-semantics — the data side of the widget library (selection,
// list contents, editable values, text). Complements ControlCascade (which
// covers their enable/disable behavior). Real controls, default font, no window.

#include <Malena/Graphics/Controls/Select.h>
#include <Malena/Graphics/Controls/RadioGroup.h>
#include <Malena/Graphics/Controls/List.h>
#include <Malena/Graphics/Controls/EditableList.h>
#include <Malena/Graphics/Text/TextInput.h>
#include <iostream>

namespace {

int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
        ++failures; \
    } \
} while (0)

// ── Select: options + selection by index ─────────────────────────────────────
void test_select_selection()
{
    ml::Select s;
    s.addOption("Zero", "v0");
    s.addOption("One",  "v1");
    s.addOption("Two",  "v2");
    CHECK(s.optionCount() == 3);

    s.selectIndex(1);
    CHECK(s.getSelectedIndex() == 1);
    CHECK(s.getSelectedValue() == "v1");
    CHECK(s.getSelectedLabel() == "One");

    s.selectIndex(2);
    CHECK(s.getSelectedValue() == "v2");   // selection moves, not accumulates
}

// ── RadioGroup: single-select exclusivity ────────────────────────────────────
void test_radiogroup_single_select()
{
    ml::RadioGroup g;
    g.addOption("A");
    g.addOption("B");
    g.addOption("C");
    CHECK(g.optionCount() == 3);

    g.select(0);
    CHECK(g.getSelectedIndex() == 0);
    g.select(2);
    CHECK(g.getSelectedIndex() == 2);      // only the last pick stays
    CHECK(g.getSelected() != nullptr);
}

// ── List: add / remove / clear row bookkeeping ───────────────────────────────
void test_list_contents()
{
    ml::List list;
    list.addItem("Wi-Fi");
    list.addItem("Bluetooth");
    list.addItem("Airplane mode");
    CHECK(list.rowCount() == 3);

    CHECK(list.removeAt(0));
    CHECK(list.rowCount() == 2);

    list.clear();
    CHECK(list.rowCount() == 0);
}

// ── EditableList: values + single-select indices ─────────────────────────────
void test_editablelist_values()
{
    ml::EditableList opts;
    opts.setSelectionMode(ml::EditableList::SelectionMode::Single);
    opts.setValues({ "Red", "Green", "Blue" }, { 1 });

    auto vals = opts.values();
    CHECK(vals.size() == 3);
    CHECK(vals[0] == "Red");
    CHECK(vals[2] == "Blue");

    auto sel = opts.selectedIndices();
    CHECK(sel.size() == 1);
    CHECK(sel[0] == 1);

    opts.setSelected(2, true);              // single-select → replaces
    auto sel2 = opts.selectedIndices();
    CHECK(sel2.size() == 1);
    CHECK(sel2[0] == 2);
}

// ── TextInput: value get/set + rich-text round-trip ──────────────────────────
void test_textinput_value()
{
    ml::TextInput input;
    input.setValue("hello world");
    CHECK(input.getValue() == "hello world");

    // Rich-text JSON round-trips the plain text content losslessly.
    std::string json = input.getRichText();
    ml::TextInput restored;
    restored.setRichText(json);
    CHECK(restored.getValue() == "hello world");

    input.clear();
    CHECK(input.getValue().empty());
}

} // namespace

int main()
{
    test_select_selection();
    test_radiogroup_single_select();
    test_list_contents();
    test_editablelist_values();
    test_textinput_value();

    if (failures == 0) { std::cout << "ControlBehavior: all checks passed\n"; return 0; }
    std::cerr << "ControlBehavior: " << failures << " check(s) failed\n";
    return 1;
}
