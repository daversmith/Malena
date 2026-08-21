// Control-level regression tests for the enable/disable behavior that real
// widgets must honor — the gaps behind the "toolbar two-clicks" bug.
//
// Uses real controls (default font via FontManager) but no window. If a control
// constructor ever starts requiring a live window/GL context this suite is the
// canary.

#include <Malena/Graphics/Controls/Select.h>
#include <Malena/Graphics/Controls/List.h>
#include <Malena/Graphics/Controls/ScrollPane.h>
#include <Malena/Graphics/Controls/SplitPanel.h>
#include <Malena/Graphics/Controls/TabbedPanel.h>
#include <Malena/Graphics/Controls/Accordion.h>
#include <Malena/Graphics/Controls/PillToggle.h>
#include <Malena/Graphics/Controls/SegmentToggle.h>
#include <Malena/Graphics/Controls/RadioButton.h>
#include <Malena/Graphics/Controls/ButtonToggle.h>
#include <Malena/Graphics/Text/TextArea.h>
#include <Malena/Core/Component.h>
#include <Malena/Engine/App/AppManager.h>
#include <iostream>

namespace {

struct Leaf : public ml::Component<>
{
    void draw(sf::RenderTarget&, sf::RenderStates) const override {}
};

int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
        ++failures; \
    } \
} while (0)

// ── Select: setEnabled must drive Flag::ENABLED (the keystone fix) ──────────
// Regression for: Select toggled only its private DISABLED flag, leaving
// Flag::ENABLED set, so a "disabled" Select still passed the click filter and
// could grab exclusive input on a toolbar click.
void test_select_setEnabled_drives_enabled_flag()
{
    ml::Select s;
    CHECK(s.isEnabled());
    CHECK(s.checkFlag(ml::Flag::ENABLED));

    s.setEnabled(false);
    CHECK(!s.isEnabled());
    CHECK(!s.checkFlag(ml::Flag::ENABLED));   // <- the bug: this used to stay true

    s.setEnabled(true);
    CHECK(s.isEnabled());
    CHECK(s.checkFlag(ml::Flag::ENABLED));
}

// A Select disabled by an ancestor cascade must also read as not-ENABLED, so the
// click filter rejects it (this is how an inactive tab's Selects go dark).
void test_select_disabled_by_parent_cascade()
{
    Leaf parent;
    ml::Select s;
    parent.addComponent(s);

    CHECK(s.checkFlag(ml::Flag::ENABLED));
    parent.setEnabled(false);
    CHECK(!s.checkFlag(ml::Flag::ENABLED));    // cascade reaches the Select
    parent.setEnabled(true);
    CHECK(s.checkFlag(ml::Flag::ENABLED));
}

// ── Every control's isEnabled() must agree with Flag::ENABLED ───────────────
// The class of bug fixed across Select / PillToggle / SegmentToggle /
// RadioButton / ButtonToggle / TextArea: setEnabled toggled only a private
// DISABLED flag, leaving Flag::ENABLED set, so the click filter (which reads
// Flag::ENABLED) still saw them as live. After setEnabled, the control's own
// disabled-state and the framework flag must never disagree.
template <typename Control>
void check_enabled_flag_invariant(const char* name)
{
    Control c;
    bool ok = c.isEnabled() && c.checkFlag(ml::Flag::ENABLED);
    c.setEnabled(false);
    ok = ok && !c.isEnabled() && !c.checkFlag(ml::Flag::ENABLED);   // filter must reject it
    c.setEnabled(true);
    ok = ok && c.isEnabled() && c.checkFlag(ml::Flag::ENABLED);
    if (!ok) { std::cerr << "FAIL: enabled/Flag::ENABLED invariant for " << name << "\n"; ++failures; }
}

void test_all_controls_enabled_flag_invariant()
{
    check_enabled_flag_invariant<ml::Select>("Select");
    check_enabled_flag_invariant<ml::PillToggle>("PillToggle");
    check_enabled_flag_invariant<ml::SegmentToggle>("SegmentToggle");
    check_enabled_flag_invariant<ml::RadioButton>("RadioButton");
    check_enabled_flag_invariant<ml::ButtonToggle>("ButtonToggle");
    check_enabled_flag_invariant<ml::TextArea>("TextArea");
}

// ── Exclusive-owner lifecycle: the actual mechanism that blocked the toolbar ──
// An open dropdown grabs exclusive input (so clicks don't fall through to what's
// behind it); closing releases it. If release ever leaks, the rest of the UI
// stays locked out — exactly the "toolbar needs two clicks" symptom.
void test_select_open_release_exclusive_owner()
{
    ml::AppManager::clearExclusiveOwner();
    ml::Select s;
    s.addOption("a", "a");
    s.addOption("b", "b");
    Leaf other;

    CHECK(ml::AppManager::exclusiveOwner() == nullptr);

    s.open();
    CHECK(ml::AppManager::isUnderExclusiveOwner(&s));        // the dropdown owns input
    CHECK(!ml::AppManager::isUnderExclusiveOwner(&other));   // everything else is locked out

    s.close();
    CHECK(ml::AppManager::exclusiveOwner() == nullptr);      // released cleanly
    CHECK(ml::AppManager::isUnderExclusiveOwner(&other));    // the rest of the UI is live again
}

// ── List: disabling the list disables its rows ──────────────────────────────
void test_list_cascade()
{
    ml::List list;
    Leaf a, b;
    list.addComponent(a);
    list.addComponent(b);

    CHECK(a.checkFlag(ml::Flag::ENABLED));
    list.setEnabled(false);
    CHECK(!a.checkFlag(ml::Flag::ENABLED));
    CHECK(!b.checkFlag(ml::Flag::ENABLED));
    list.setEnabled(true);
    CHECK(a.checkFlag(ml::Flag::ENABLED));
}

// ── ScrollPane: disabling the pane must disable its scrolled children ────────
// Regression for the quiz-list gap: ScrollPane kept a bespoke child list and did
// not forward enabled-state, so its contents stayed click-testable when the
// surrounding tab was disabled.
void test_scrollpane_cascade()
{
    ml::ScrollPane pane(200.f, 200.f);
    Leaf child;
    pane.addComponent(child);

    CHECK(child.checkFlag(ml::Flag::ENABLED));
    pane.setEnabled(false);
    CHECK(!child.checkFlag(ml::Flag::ENABLED));   // must reach scrolled children
    pane.setEnabled(true);
    CHECK(child.checkFlag(ml::Flag::ENABLED));
}

// ── SplitPanel: disabling the split disables its panes ───────────────────────
void test_splitpanel_cascade()
{
    ml::SplitPanel split;
    auto& a = split.addPane(std::make_unique<Leaf>(), 100.f);
    auto& b = split.addPane(std::make_unique<Leaf>());

    CHECK(a.checkFlag(ml::Flag::ENABLED));
    split.setEnabled(false);
    CHECK(!a.checkFlag(ml::Flag::ENABLED));
    CHECK(!b.checkFlag(ml::Flag::ENABLED));
    split.setEnabled(true);
    CHECK(a.checkFlag(ml::Flag::ENABLED));
    CHECK(b.checkFlag(ml::Flag::ENABLED));
}

// ── TabbedPanel: only the active tab's content is live ───────────────────────
// The toolbar bug lived in a tab container: a hidden tab's content stayed
// click-testable. Two invariants here: (1) an inactive tab's content is NOT
// enabled (so it can't be clicked through the active panel), and (2) disabling
// the whole panel darkens the active tab too; re-enabling restores it while the
// inactive tab stays dark.
void test_tabbedpanel_cascade()
{
    ml::TabbedPanel tabs;
    auto& first  = tabs.addTab("A", std::make_unique<Leaf>());   // active
    auto& second = tabs.addTab("B", std::make_unique<Leaf>());   // inactive

    CHECK(first.checkFlag(ml::Flag::ENABLED));     // active tab is live
    CHECK(!second.checkFlag(ml::Flag::ENABLED));   // inactive tab is NOT click-testable

    tabs.setEnabled(false);
    CHECK(!first.checkFlag(ml::Flag::ENABLED));    // disabling the panel darkens active content
    CHECK(!second.checkFlag(ml::Flag::ENABLED));

    tabs.setEnabled(true);
    CHECK(first.checkFlag(ml::Flag::ENABLED));     // active tab live again
    CHECK(!second.checkFlag(ml::Flag::ENABLED));   // inactive tab stays dark

    tabs.selectTab(1);                              // switch active tab
    CHECK(second.checkFlag(ml::Flag::ENABLED));    // now-active tab becomes live
    CHECK(!first.checkFlag(ml::Flag::ENABLED));    // now-inactive tab goes dark
}

// ── Accordion: disabling the accordion disables every section's header + list ─
void test_accordion_cascade()
{
    ml::Accordion acc;
    auto s0 = acc.addSection("One", 120.f);
    auto s1 = acc.addSection("Two", 120.f);

    CHECK(s0.header.checkFlag(ml::Flag::ENABLED));
    CHECK(s0.list.checkFlag(ml::Flag::ENABLED));
    acc.setEnabled(false);
    CHECK(!s0.header.checkFlag(ml::Flag::ENABLED));
    CHECK(!s0.list.checkFlag(ml::Flag::ENABLED));
    CHECK(!s1.header.checkFlag(ml::Flag::ENABLED));
    CHECK(!s1.list.checkFlag(ml::Flag::ENABLED));
    acc.setEnabled(true);
    CHECK(s0.header.checkFlag(ml::Flag::ENABLED));
    CHECK(s1.list.checkFlag(ml::Flag::ENABLED));
}

// ── A disabled Select must not open (so it can't grab exclusive input) ───────
// open() is now gated on Flag::ENABLED, closing the gap where calling open()
// directly on a disabled Select would still grab ownership.
void test_disabled_select_cannot_open()
{
    ml::AppManager::clearExclusiveOwner();
    ml::Select s;
    s.addOption("a", "a");
    s.setEnabled(false);
    s.open();
    CHECK(ml::AppManager::exclusiveOwner() == nullptr);   // never grabbed input
    s.setEnabled(true);
    s.open();
    CHECK(ml::AppManager::isUnderExclusiveOwner(&s));      // enabled one still opens
    s.close();
    ml::AppManager::clearExclusiveOwner();
}

} // namespace

int main()
{
    test_select_setEnabled_drives_enabled_flag();
    test_select_disabled_by_parent_cascade();
    test_all_controls_enabled_flag_invariant();
    test_select_open_release_exclusive_owner();
    test_disabled_select_cannot_open();
    test_list_cascade();
    test_scrollpane_cascade();
    test_splitpanel_cascade();
    test_tabbedpanel_cascade();
    test_accordion_cascade();

    if (failures == 0) {
        std::cout << "ControlCascade: all checks passed\n";
        return 0;
    }
    std::cerr << "ControlCascade: " << failures << " check(s) failed\n";
    return 1;
}
