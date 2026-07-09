// Retained-anchor invariants — the Align verbs are sticky by default and re-solve
// on AnchorManager::solveAll(): a moved reference drags its dependents, a direct
// setPosition detaches (manual wins), unanchored() opts out, and destroying a
// referenced object scrubs the dangling anchor. Window-free: every anchor here is
// against another component (no window reference), and solveAll() is called
// explicitly rather than via a resize event.

#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Layout/AnchorManager.h>
#include <cmath>
#include <iostream>

namespace {

int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
        ++failures; \
    } \
} while (0)

bool near(float a, float b, float eps = 0.5f) { return std::fabs(a - b) < eps; }

ml::Rectangle makeRect(float x, float y, float w, float h)
{
    ml::Rectangle r;
    r.setSize({w, h});
    r.setPosition({x, y});
    return r;
}

// ── A moved reference drags its dependent on the next solve ──────────────────
void test_reference_moves_dependent()
{
    ml::Rectangle a = makeRect(0.f, 0.f, 10.f, 10.f);
    ml::Rectangle b = makeRect(0.f, 0.f, 10.f, 10.f);

    b.setBelow(a);                            // retained + applied now
    CHECK(near(b.getPosition().y, 10.f));
    CHECK(ml::AnchorManager::hasAnchors(&b));

    a.setPosition({0.f, 100.f});              // move the reference
    ml::AnchorManager::solveAll();
    CHECK(near(b.getPosition().y, 110.f));    // b followed a
}

// ── A direct setPosition detaches (manual control wins) ──────────────────────
void test_manual_move_detaches()
{
    ml::Rectangle a = makeRect(0.f, 0.f, 10.f, 10.f);
    ml::Rectangle b = makeRect(0.f, 0.f, 10.f, 10.f);

    b.setBelow(a);
    b.setPosition({50.f, 50.f});              // user takes manual control
    ml::AnchorManager::solveAll();
    CHECK(near(b.getPosition().x, 50.f));     // not snapped back
    CHECK(near(b.getPosition().y, 50.f));
    CHECK(!ml::AnchorManager::hasAnchors(&b));// anchor was dropped
}

// ── unanchored() freezes the current position ────────────────────────────────
void test_unanchored_optout()
{
    ml::Rectangle a = makeRect(0.f, 0.f, 10.f, 10.f);
    ml::Rectangle b = makeRect(0.f, 0.f, 10.f, 10.f);

    b.setBelow(a);
    b.unanchored();
    CHECK(!ml::AnchorManager::hasAnchors(&b));
    a.setPosition({0.f, 100.f});
    ml::AnchorManager::solveAll();
    CHECK(near(b.getPosition().y, 10.f));     // stayed put, anchor gone
}

// ── Single-axis ops compose: centerX + centerY both retained and re-solved ───
// (Note: setBelow/setAbove/setLeftOf/setRightOf are two-axis in Align — they also
//  align the cross axis to the reference — so genuine composition uses the
//  single-axis center helpers.)
void test_compose_axes()
{
    ml::Rectangle parent = makeRect(0.f, 0.f, 100.f, 100.f);
    ml::Rectangle c      = makeRect(0.f, 0.f, 10.f, 10.f);

    c.centerHorizonally(parent);              // x anchor
    c.centerVertically(parent);               // y anchor
    CHECK(near(c.getPosition().x, 45.f));     // (100-10)/2
    CHECK(near(c.getPosition().y, 45.f));

    parent.setPosition({200.f, 100.f});       // move the reference on both axes
    ml::AnchorManager::solveAll();
    CHECK(near(c.getPosition().x, 245.f));    // re-centered horizontally
    CHECK(near(c.getPosition().y, 145.f));    // re-centered vertically
}

// ── Fixpoint: a chain settles regardless of solve order ──────────────────────
void test_dependency_chain()
{
    ml::Rectangle a = makeRect(0.f, 0.f, 10.f, 10.f);
    ml::Rectangle b = makeRect(0.f, 0.f, 10.f, 10.f);
    ml::Rectangle c = makeRect(0.f, 0.f, 10.f, 10.f);

    b.setBelow(a);          // b under a
    c.setBelow(b);          // c under b
    a.setPosition({0.f, 100.f});
    ml::AnchorManager::solveAll();
    CHECK(near(b.getPosition().y, 110.f));
    CHECK(near(c.getPosition().y, 120.f));    // settled through the chain
}

// ── Re-issuing the same verb does not accumulate anchors ─────────────────────
void test_dedup()
{
    ml::Rectangle a = makeRect(0.f, 0.f, 10.f, 10.f);
    ml::Rectangle b = makeRect(0.f, 0.f, 10.f, 10.f);

    b.setBelow(a); b.setBelow(a); b.setBelow(a);   // e.g. fill() re-running
    a.setPosition({0.f, 100.f});
    ml::AnchorManager::solveAll();
    CHECK(near(b.getPosition().y, 110.f));         // one clean effect, not thrice
}

// ── Destroying a referenced object scrubs the dangling anchor ────────────────
void test_scrub_on_destroy()
{
    ml::Rectangle b = makeRect(0.f, 0.f, 10.f, 10.f);
    {
        ml::Rectangle a = makeRect(0.f, 0.f, 10.f, 10.f);
        b.setBelow(a);
        CHECK(ml::AnchorManager::hasAnchors(&b));
    } // a destroyed — its ~Positionable scrubs b's anchor to it

    CHECK(!ml::AnchorManager::hasAnchors(&b));
    ml::AnchorManager::solveAll();             // must not touch freed memory
    CHECK(true);
}

} // namespace

int main()
{
    test_reference_moves_dependent();
    test_manual_move_detaches();
    test_unanchored_optout();
    test_compose_axes();
    test_dependency_chain();
    test_dedup();
    test_scrub_on_destroy();

    if (failures == 0) { std::cout << "Anchors: all checks passed\n"; return 0; }
    std::cerr << "Anchors: " << failures << " check(s) failed\n";
    return 1;
}
