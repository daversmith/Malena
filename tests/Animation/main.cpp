// Animation subsystem invariants — Easing curves, the generic Tweener<T> value
// animator, and AnimationManager scheduling/cancellation. Deterministic and
// window-free: every test drives time with explicit dt steps.

#include <Malena/Animation/Easing.h>
#include <Malena/Animation/Tweener.h>
#include <Malena/Animation/AnimationManager.h>
#include <Malena/Animation/Animate.h>
#include <Malena/Graphics/Primitives/Rectangle.h>
#include <SFML/Graphics/Color.hpp>
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

bool near(float a, float b, float eps = 0.001f) { return std::fabs(a - b) < eps; }

// ── Easing endpoints + monotonic midpoints ───────────────────────────────────
void test_easing_endpoints()
{
    // Every curve pins 0→0 and 1→1 (Back/Elastic overshoot between, but land right).
    ml::Easing::Fn fns[] = {
        ml::Easing::Linear,
        ml::Easing::EaseInQuad,  ml::Easing::EaseOutQuad,  ml::Easing::EaseInOutQuad,
        ml::Easing::EaseInCubic, ml::Easing::EaseOutCubic, ml::Easing::EaseInOutCubic,
        ml::Easing::EaseInExpo,  ml::Easing::EaseOutExpo,  ml::Easing::EaseInOutExpo,
        ml::Easing::EaseInBack,  ml::Easing::EaseOutBack,
        ml::Easing::EaseInElastic, ml::Easing::EaseOutElastic,
        ml::Easing::EaseOutBounce, ml::Easing::EaseInBounce,
    };
    for (auto f : fns) { CHECK(near(f(0.f), 0.f, 0.002f)); CHECK(near(f(1.f), 1.f, 0.002f)); }

    // Linear is the identity; EaseOutQuad is ahead of linear at the midpoint.
    CHECK(near(ml::Easing::Linear(0.5f), 0.5f));
    CHECK(ml::Easing::EaseOutQuad(0.5f) > 0.5f);
    CHECK(ml::Easing::EaseInQuad(0.5f)  < 0.5f);
}

// ── Tweener<float>: linear midpoint + completion callback fires once ─────────
void test_tweener_float()
{
    float v = -1.f; int completes = 0;
    ml::Tweener<float> t(0.f, 10.f, 1.f, ml::Easing::Linear);
    t.onUpdate([&](const float& x){ v = x; });
    t.onComplete([&]{ ++completes; });

    t.update(0.25f); CHECK(near(v, 2.5f));
    t.update(0.25f); CHECK(near(v, 5.0f));
    CHECK(!t.done());
    t.update(0.5f);  CHECK(near(v, 10.f));  CHECK(t.done()); CHECK(completes == 1);

    t.update(1.f);   CHECK(completes == 1);   // no double-fire after completion
}

// ── Framerate independence: same wall-clock ⇒ same value, any step count ─────
void test_tweener_dt_independent()
{
    auto endValue = [](int steps) {
        ml::Tweener<float> t(0.f, 100.f, 1.f, ml::Easing::EaseInOutCubic);
        float last = 0.f;
        t.onUpdate([&](const float& x){ last = x; });
        const float dt = 1.f / steps;
        for (int i = 0; i < steps; ++i) t.update(dt);
        return last;
    };
    CHECK(near(endValue(10), endValue(1000), 0.01f));   // 10fps vs 1000fps agree
}

// ── Delay holds at the start value, then plays ───────────────────────────────
void test_tweener_delay()
{
    float v = -1.f;
    ml::Tweener<float> t(0.f, 10.f, 1.f, ml::Easing::Linear);
    t.delay(0.5f).onUpdate([&](const float& x){ v = x; });

    t.update(0.25f); CHECK(near(v, 0.f));   // still in the delay window
    t.update(0.25f); CHECK(near(v, 0.f));   // delay exactly consumed
    t.update(0.5f);  CHECK(near(v, 5.f));   // half a second of actual tween
}

// ── Ping-pong (no loop): goes out and comes back, then completes ─────────────
void test_tweener_pingpong()
{
    float v = 0.f;
    ml::Tweener<float> t(0.f, 10.f, 1.f, ml::Easing::Linear);
    t.pingPong().onUpdate([&](const float& x){ v = x; });

    t.update(1.0f); CHECK(near(v, 10.f));   // out
    t.update(0.5f); CHECK(near(v, 5.f));    // coming back
    t.update(0.5f); CHECK(near(v, 0.f));    // home
    CHECK(t.done());
}

// ── Color lerp is component-wise ─────────────────────────────────────────────
void test_tweener_color()
{
    sf::Color c;
    ml::Tweener<sf::Color> t(sf::Color(0,0,0,0), sf::Color(100,200,50,255), 1.f, ml::Easing::Linear);
    t.onUpdate([&](const sf::Color& x){ c = x; });
    t.update(0.5f);
    CHECK(c.r == 50); CHECK(c.g == 100); CHECK(c.b == 25); CHECK(c.a == 128);
}

// ── AnimationManager drives a registered step and drops it when done ─────────
void test_manager_scheduling()
{
    ml::AnimationManager::cancel(nullptr);   // clean slate for this owner tag
    const int ownerA = 0, ownerB = 0;

    float a = 0.f;
    auto ta = std::make_shared<ml::Tweener<float>>(0.f, 1.f, 1.f, ml::Easing::Linear);
    ta->onUpdate([&](const float& x){ a = x; });
    ml::AnimationManager::add(&ownerA, [ta](float dt){ ta->update(dt); return ta->done(); });

    CHECK(ml::AnimationManager::hasActive(&ownerA));
    ml::AnimationManager::advance(0.5f); CHECK(near(a, 0.5f));
    ml::AnimationManager::advance(0.5f); CHECK(near(a, 1.0f));
    CHECK(!ml::AnimationManager::hasActive(&ownerA));   // auto-removed on completion

    // Cancel removes a live animation without completing it.
    float b = 0.f;
    auto tb = std::make_shared<ml::Tweener<float>>(0.f, 1.f, 1.f, ml::Easing::Linear);
    tb->onUpdate([&](const float& x){ b = x; });
    ml::AnimationManager::add(&ownerB, [tb](float dt){ tb->update(dt); return tb->done(); });
    ml::AnimationManager::advance(0.25f);
    ml::AnimationManager::cancel(&ownerB);
    CHECK(!ml::AnimationManager::hasActive(&ownerB));
    ml::AnimationManager::advance(1.f); CHECK(near(b, 0.25f));   // frozen where cancelled
}

// ── Animate facade: move + moveBy + auto-cancel on destruction ───────────────
void test_animate_facade()
{
    ml::Rectangle r;
    r.setSize({10.f, 10.f});
    r.setPosition({0.f, 0.f});

    bool done = false;
    r.animate().move({100.f, 40.f}, 1.f, ml::Easing::Linear, [&]{ done = true; });
    ml::AnimationManager::advance(0.5f);
    CHECK(near(r.getPosition().x, 50.f)); CHECK(near(r.getPosition().y, 20.f));
    ml::AnimationManager::advance(0.5f);
    CHECK(near(r.getPosition().x, 100.f)); CHECK(done);

    // moveBy is relative to the current position.
    r.animate().moveBy({-100.f, 0.f}, 1.f, ml::Easing::Linear);
    ml::AnimationManager::advance(1.f);
    CHECK(near(r.getPosition().x, 0.f));

    // Destroying the owner cancels its animations (no dangling setter next frame).
    {
        ml::Rectangle tmp;
        tmp.setPosition({0.f, 0.f});
        tmp.animate().move({500.f, 0.f}, 1.f, ml::Easing::Linear);
        CHECK(tmp.animate().active());
    } // tmp destroyed here
    ml::AnimationManager::advance(1.f);   // must not crash / touch freed memory
    CHECK(true);
}

} // namespace

int main()
{
    test_easing_endpoints();
    test_tweener_float();
    test_tweener_dt_independent();
    test_tweener_delay();
    test_tweener_pingpong();
    test_tweener_color();
    test_manager_scheduling();
    test_animate_facade();

    if (failures == 0) { std::cout << "Animation: all checks passed\n"; return 0; }
    std::cerr << "Animation: " << failures << " check(s) failed\n";
    return 1;
}
