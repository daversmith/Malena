// Core parent/child topology — the per-instance _children/_parent model that
// replaced the old static _childMap. Locks reparenting, removal, effective
// visibility through the ancestor chain, and the topmostMatching hit-test seam.

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

// ── Single-parent: addComponent moves a child off its previous parent ────────
void test_reparent_moves_child()
{
    Leaf p1, p2, child;
    p1.addComponent(child);
    p2.addComponent(child);   // silently re-parents

    // child now belongs to p2 only: disabling p1 must not reach it.
    p1.setEnabled(false);
    CHECK(child.checkFlag(ml::Flag::ENABLED));
    // disabling p2 (its real parent) does.
    p2.setEnabled(false);
    CHECK(!child.checkFlag(ml::Flag::ENABLED));
}

// ── removeComponent detaches from the cascade ────────────────────────────────
void test_remove_detaches()
{
    Leaf p, child;
    p.addComponent(child);
    p.removeComponent(child);

    p.setEnabled(false);
    CHECK(child.checkFlag(ml::Flag::ENABLED));   // no longer a child → unaffected
}

// ── isEffectivelyVisible walks the ancestor chain ────────────────────────────
void test_effective_visibility_chain()
{
    Leaf grandparent, parent, child;
    grandparent.addComponent(parent);
    parent.addComponent(child);

    CHECK(child.isEffectivelyVisible());

    grandparent.setVisible(false);
    CHECK(child.isVisible());                 // its OWN flag is untouched
    CHECK(!child.isEffectivelyVisible());     // but an ancestor is hidden

    grandparent.setVisible(true);
    CHECK(child.isEffectivelyVisible());
}

// ── topmostMatching returns the front-most accepted node, skipping hidden ────
void test_topmost_matching()
{
    Leaf parent, low, high;
    parent.addComponent(low,  0);     // back
    parent.addComponent(high, 100);   // front (higher layer paints last)

    // accept-all → the front-most descendant is returned first.
    auto* top = parent.topmostMatching([](ml::Core&){ return true; });
    CHECK(top == &high);

    // hide the front one → traversal skips it, next match is the back one.
    high.setVisible(false);
    auto* top2 = parent.topmostMatching([&parent](ml::Core& c){ return &c != &parent; });
    CHECK(top2 == &low);

    // a predicate that only accepts a specific node finds exactly it.
    high.setVisible(true);
    auto* only = parent.topmostMatching([&](ml::Core& c){ return &c == &low; });
    CHECK(only == &low);
}

} // namespace

int main()
{
    test_reparent_moves_child();
    test_remove_detaches();
    test_effective_visibility_chain();
    test_topmost_matching();

    if (failures == 0) { std::cout << "CoreTopology: all checks passed\n"; return 0; }
    std::cerr << "CoreTopology: " << failures << " check(s) failed\n";
    return 1;
}
