// Verifies the enable/disable cascade that ChatWindow (and any composite
// component with member children) depends on after removing its bespoke
// setEnabled / setParentEnabled overrides.
//
// The test exercises Core's _childMap-based cascade directly, so it does not
// need a window, fonts, or any SFML resources.

#include <Malena/Core/Component.h>
#include <cassert>
#include <iostream>

namespace {

struct Leaf : public ml::Component<>
{
    void draw(sf::RenderTarget&, sf::RenderStates) const override {}
};

// Composite mimics the ChatWindow shape: two leaf children registered as
// members via addComponents().
struct Composite : public ml::Component<>
{
    Leaf a;
    Leaf b;
    Composite() { addComponents(a, b); }
    void draw(sf::RenderTarget&, sf::RenderStates) const override {}
};

int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
        ++failures; \
    } \
} while (0)

void test_cascade_disables_children()
{
    Composite c;
    CHECK(c.isEnabled());
    CHECK(c.a.isEnabled());
    CHECK(c.b.isEnabled());

    c.setEnabled(false);
    CHECK(!c.isEnabled());
    CHECK(!c.a.isEnabled());
    CHECK(!c.b.isEnabled());

    c.setEnabled(true);
    CHECK(c.isEnabled());
    CHECK(c.a.isEnabled());
    CHECK(c.b.isEnabled());
}

// The bug the bespoke overrides introduced: when the parent is re-enabled, an
// independently-disabled child would be force-enabled. The cascade through
// _childMap touches only _parentEnabled, so this scenario must round-trip.
void test_independent_child_disable_survives_parent_toggle()
{
    Composite c;
    c.a.setEnabled(false);          // user explicitly disables a
    CHECK(!c.a.isEnabled());
    CHECK(c.b.isEnabled());

    c.setEnabled(false);             // parent off — both effectively off
    CHECK(!c.a.isEnabled());
    CHECK(!c.b.isEnabled());

    c.setEnabled(true);              // parent back on
    CHECK(!c.a.isEnabled());         // a stays off (its _selfEnabled was false)
    CHECK(c.b.isEnabled());          // b returns to enabled

    c.a.setEnabled(true);            // user re-enables a
    CHECK(c.a.isEnabled());
}

// Composite-of-composite: setEnabled on the outer should propagate two levels.
void test_nested_cascade()
{
    struct Outer : public ml::Component<>
    {
        Composite inner;
        Outer() { addComponent(inner); }
        void draw(sf::RenderTarget&, sf::RenderStates) const override {}
    };

    Outer o;
    CHECK(o.inner.a.isEnabled());

    o.setEnabled(false);
    CHECK(!o.isEnabled());
    CHECK(!o.inner.isEnabled());
    CHECK(!o.inner.a.isEnabled());
    CHECK(!o.inner.b.isEnabled());

    o.setEnabled(true);
    CHECK(o.inner.a.isEnabled());
    CHECK(o.inner.b.isEnabled());
}

// addComponents must accept any pack of Core-derived references and register
// each one — equivalent to calling addComponent N times.
void test_addComponents_matches_individual_calls()
{
    Leaf parentA, childA1, childA2;
    parentA.addComponent(childA1);
    parentA.addComponent(childA2);
    parentA.setEnabled(false);

    Leaf parentB, childB1, childB2;
    parentB.addComponents(childB1, childB2);
    parentB.setEnabled(false);

    CHECK(parentA.isEnabled() == parentB.isEnabled());
    CHECK(childA1.isEnabled() == childB1.isEnabled());
    CHECK(childA2.isEnabled() == childB2.isEnabled());
    CHECK(!childB1.isEnabled());
    CHECK(!childB2.isEnabled());
}

} // namespace

int main()
{
    test_cascade_disables_children();
    test_independent_child_disable_survives_parent_toggle();
    test_nested_cascade();
    test_addComponents_matches_individual_calls();

    if (failures == 0) {
        std::cout << "EnableCascade: all checks passed\n";
        return 0;
    }
    std::cerr << "EnableCascade: " << failures << " check(s) failed\n";
    return 1;
}
