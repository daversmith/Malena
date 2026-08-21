// Positionable invariants — position, bounds, relative-layout helpers, and
// delta-time position animation. Uses ml::Rectangle (a concrete shape implementing
// the setPosition/getPosition/getGlobalBounds contract). Window-free.

#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Animation/Animate.h>
#include <Malena/Animation/AnimationManager.h>
#include <Malena/Animation/Easing.h>
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

// ── Position + bounds track each other ───────────────────────────────────────
void test_position_and_bounds()
{
    ml::Rectangle r;
    r.setSize({120.f, 40.f});
    r.setPosition({100.f, 200.f});

    CHECK(near(r.getPosition().x, 100.f));
    CHECK(near(r.getPosition().y, 200.f));

    auto b = r.getGlobalBounds();
    CHECK(near(b.position.x, 100.f));
    CHECK(near(b.position.y, 200.f));
    CHECK(near(b.size.x, 120.f));
    CHECK(near(b.size.y, 40.f));

    r.setPosition({300.f, 50.f});
    CHECK(near(r.getGlobalBounds().position.x, 300.f));   // bounds follow position
    CHECK(near(r.getGlobalBounds().position.y, 50.f));
}

// ── Relative-layout helpers compute deterministic geometry ───────────────────
void test_relative_layout()
{
    ml::Rectangle a, b;
    a.setSize({100.f, 50.f});
    a.setPosition({0.f, 0.f});
    b.setSize({30.f, 30.f});

    b.setRightOf(a, 10.f);
    CHECK(near(b.getPosition().x, 110.f));   // a.right (100) + gap (10)

    b.setBelow(a, 5.f);
    CHECK(near(b.getPosition().y, 55.f));     // a.bottom (50) + gap (5)

    b.center(a);
    CHECK(near(b.getPosition().x, 35.f));     // (100-30)/2
    CHECK(near(b.getPosition().y, 10.f));     // (50-30)/2
}

// ── Animate: position tween drives setPosition over delta time ───────────────
void test_animate_position()
{
    ml::Rectangle r;
    r.setSize({10.f, 10.f});
    r.setPosition({0.f, 0.f});

    CHECK(!r.animate().active());                          // idle at rest
    r.animate().move({100.f, 0.f}, 1.f, ml::Easing::Linear);
    CHECK(r.animate().active());                           // running

    ml::AnimationManager::advance(0.5f);                   // halfway (linear)
    CHECK(near(r.getPosition().x, 50.f));

    ml::AnimationManager::advance(0.5f);                   // reaches the end
    CHECK(near(r.getPosition().x, 100.f));
    CHECK(!r.animate().active());                          // finished → idle
}

} // namespace

int main()
{
    test_position_and_bounds();
    test_relative_layout();
    test_animate_position();

    if (failures == 0) { std::cout << "Positionable: all checks passed\n"; return 0; }
    std::cerr << "Positionable: " << failures << " check(s) failed\n";
    return 1;
}
