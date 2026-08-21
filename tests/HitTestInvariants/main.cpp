// Regression tests for the invariants the click filter depends on — the exact
// properties whose violation produced the "toolbar needs two clicks" class of
// bugs (origin-parked / hidden-ancestor / not-under-owner components silently
// staying click-testable).
//
// These exercise Core / AppManager logic directly, so — like the other suites —
// they need no window, fonts, or SFML resources. (isHovered() itself is pure
// SFML geometry and is left to manual/UI testing; here we pin the *gates* the
// dispatcher composes: Flag::ENABLED, isEffectivelyVisible(), and
// AppManager::isUnderExclusiveOwner().)

#include <Malena/Core/Component.h>
#include <Malena/Engine/App/AppManager.h>
#include <cassert>
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

// ── Gate 1: enabled / Flag::ENABLED must never disagree ─────────────────────
// The click filter rejects on !checkFlag(Flag::ENABLED). If a control's own
// isEnabled()/disabled-state ever diverges from that flag, it passes the filter
// while "looking" disabled — exactly the Select::setEnabled bug. For any Core,
// the two must agree.
void test_enabled_flag_matches_state()
{
    Leaf l;
    CHECK(l.isEnabled());
    CHECK(l.checkFlag(ml::Flag::ENABLED));

    l.setEnabled(false);
    CHECK(!l.isEnabled());
    CHECK(!l.checkFlag(ml::Flag::ENABLED));   // the gate the dispatcher reads

    l.setEnabled(true);
    CHECK(l.isEnabled());
    CHECK(l.checkFlag(ml::Flag::ENABLED));
}

// ── Gate 2: effective visibility walks the parent chain ─────────────────────
// A component inside a hidden container is not drawn and must not be
// hit-testable, even though its *own* HIDDEN flag still says visible.
void test_effective_visibility_through_ancestor()
{
    Leaf parent, child;
    parent.addComponent(child);

    CHECK(child.isEffectivelyVisible());

    parent.setVisible(false);                 // hide the container only
    CHECK(child.isVisible());                 // child's own flag is untouched...
    CHECK(!child.isEffectivelyVisible());     // ...but it is effectively hidden

    parent.setVisible(true);
    CHECK(child.isEffectivelyVisible());

    child.setVisible(false);                  // hide the child itself
    CHECK(!child.isEffectivelyVisible());
}

void test_effective_visibility_two_levels()
{
    Leaf grandparent, parent, child;
    grandparent.addComponent(parent);
    parent.addComponent(child);

    CHECK(child.isEffectivelyVisible());
    grandparent.setVisible(false);            // hide two levels up
    CHECK(!child.isEffectivelyVisible());
    grandparent.setVisible(true);
    CHECK(child.isEffectivelyVisible());
}

// ── Gate 3: exclusive-owner gating ──────────────────────────────────────────
// With an owner set, only the owner and its descendants are interactive; an
// unrelated component must be locked out. This is what blocks the background
// while a Modal/Select/dropdown is open.
void test_exclusive_owner_gating()
{
    Leaf owner, ownerChild, unrelated;
    owner.addComponent(ownerChild);

    // No owner → everything is "under" the (absent) owner.
    ml::AppManager::clearExclusiveOwner();
    CHECK(ml::AppManager::isUnderExclusiveOwner(&owner));
    CHECK(ml::AppManager::isUnderExclusiveOwner(&unrelated));

    ml::AppManager::setExclusiveOwner(&owner);
    CHECK(ml::AppManager::isUnderExclusiveOwner(&owner));        // the owner itself
    CHECK(ml::AppManager::isUnderExclusiveOwner(&ownerChild));   // a descendant
    CHECK(!ml::AppManager::isUnderExclusiveOwner(&unrelated));   // locked out

    ml::AppManager::clearExclusiveOwner();
    CHECK(ml::AppManager::isUnderExclusiveOwner(&unrelated));    // lifted
}

// Re-parenting must keep the descendant relationship correct for ownership.
void test_exclusive_owner_after_reparent()
{
    Leaf ownerA, ownerB, child;
    ownerA.addComponent(child);

    ml::AppManager::setExclusiveOwner(&ownerA);
    CHECK(ml::AppManager::isUnderExclusiveOwner(&child));

    ownerB.addComponent(child);   // soft single-parent: moves child to ownerB
    CHECK(!ml::AppManager::isUnderExclusiveOwner(&child));  // no longer under ownerA

    ml::AppManager::setExclusiveOwner(&ownerB);
    CHECK(ml::AppManager::isUnderExclusiveOwner(&child));

    ml::AppManager::clearExclusiveOwner();
}

} // namespace

int main()
{
    test_enabled_flag_matches_state();
    test_effective_visibility_through_ancestor();
    test_effective_visibility_two_levels();
    test_exclusive_owner_gating();
    test_exclusive_owner_after_reparent();

    if (failures == 0) {
        std::cout << "HitTestInvariants: all checks passed\n";
        return 0;
    }
    std::cerr << "HitTestInvariants: " << failures << " check(s) failed\n";
    return 1;
}
