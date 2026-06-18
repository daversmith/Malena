// Control-level regression tests for the enable/disable behavior that real
// widgets must honor — the gaps behind the "toolbar two-clicks" bug.
//
// Uses real controls (default font via FontManager) but no window. If a control
// constructor ever starts requiring a live window/GL context this suite is the
// canary.

#include <Malena/Graphics/Controls/Select.h>
#include <Malena/Graphics/Controls/List.h>
#include <Malena/Graphics/Controls/ScrollPane.h>
#include <Malena/Core/Component.h>
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

} // namespace

int main()
{
    test_select_setEnabled_drives_enabled_flag();
    test_select_disabled_by_parent_cascade();
    test_list_cascade();
    test_scrollpane_cascade();

    if (failures == 0) {
        std::cout << "ControlCascade: all checks passed\n";
        return 0;
    }
    std::cerr << "ControlCascade: " << failures << " check(s) failed\n";
    return 1;
}
