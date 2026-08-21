// Verifies the layer-key ordering API introduced in Phase 2.
//
// Today's _children vector is stable-sorted by layer ascending; within a
// layer, registration order wins. This test pins those guarantees by
// inspecting Core::getChildren() through a thin Component<> probe.

#include <Malena/Core/Component.h>
#include <cassert>
#include <iostream>

namespace {

struct Leaf : public ml::Component<>
{
    void draw(sf::RenderTarget&, sf::RenderStates) const override {}
};

struct Probe : public ml::Component<>
{
    void draw(sf::RenderTarget&, sf::RenderStates) const override {}

    // Re-export protected accessor so the test can inspect ordering.
    using ml::Core::getChildren;
};

int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
        ++failures; \
    } \
} while (0)

void test_explicit_layer_orders_ascending()
{
    Probe p;
    Leaf bg, content, overlay;

    p.addComponent(overlay, 200);
    p.addComponent(content, 100);
    p.addComponent(bg,        0);

    const auto& k = p.getChildren();
    CHECK(k.size() == 3);
    CHECK(k[0].component == &bg      && k[0].layer == 0);
    CHECK(k[1].component == &content && k[1].layer == 100);
    CHECK(k[2].component == &overlay && k[2].layer == 200);
}

void test_within_layer_keeps_insertion_order()
{
    Probe p;
    Leaf a, b, c;

    p.addComponent(a, 100);
    p.addComponent(b, 100);
    p.addComponent(c, 100);

    const auto& k = p.getChildren();
    CHECK(k[0].component == &a);
    CHECK(k[1].component == &b);
    CHECK(k[2].component == &c);
}

void test_default_layer_is_100()
{
    Probe p;
    Leaf def, low, high;

    p.addComponent(def);          // default
    p.addComponent(low,  50);
    p.addComponent(high, 150);

    const auto& k = p.getChildren();
    CHECK(k[0].component == &low);
    CHECK(k[1].component == &def && k[1].layer == 100);
    CHECK(k[2].component == &high);
}

void test_no_arg_addComponent_still_orders_by_insertion()
{
    // Backwards-compatible path: only addComponent(child) calls. Everything
    // lands on DefaultLayer, so insertion order = draw order.
    Probe p;
    Leaf a, b, c;
    p.addComponent(a);
    p.addComponent(b);
    p.addComponent(c);

    const auto& k = p.getChildren();
    CHECK(k[0].component == &a);
    CHECK(k[1].component == &b);
    CHECK(k[2].component == &c);
}

void test_readding_same_child_updates_layer()
{
    // Spec: calling addComponent again on the same child changes its layer.
    // Useful for runtime reordering ("bring this overlay to front").
    Probe p;
    Leaf a, b;

    p.addComponent(a, 100);
    p.addComponent(b, 200);

    CHECK(p.getChildren()[0].component == &a);
    CHECK(p.getChildren()[1].component == &b);

    p.addComponent(a, 300);   // promote a above b

    const auto& k = p.getChildren();
    CHECK(k.size() == 2);
    CHECK(k[0].component == &b);
    CHECK(k[1].component == &a && k[1].layer == 300);
}

void test_addComponents_variadic_default_layer()
{
    Probe p;
    Leaf a, b, c;
    p.addComponents(a, b, c);

    const auto& k = p.getChildren();
    CHECK(k.size() == 3);
    CHECK(k[0].component == &a && k[0].layer == 100);
    CHECK(k[1].component == &b);
    CHECK(k[2].component == &c);
}

void test_addComponents_variadic_shared_layer()
{
    Probe p;
    Leaf bg, fg1, fg2;
    p.addComponent(bg, 0);
    p.addComponents(200, fg1, fg2);

    const auto& k = p.getChildren();
    CHECK(k.size() == 3);
    CHECK(k[0].component == &bg  && k[0].layer == 0);
    CHECK(k[1].component == &fg1 && k[1].layer == 200);
    CHECK(k[2].component == &fg2 && k[2].layer == 200);
}

// ── Phase 3 — enum overloads ───────────────────────────────────────────────

void test_framework_ml_layer_enum()
{
    Probe p;
    Leaf bg, content, overlay;

    p.addComponent(overlay, ml::Layer::Overlay);
    p.addComponent(content, ml::Layer::Content);
    p.addComponent(bg,      ml::Layer::Background);

    const auto& k = p.getChildren();
    CHECK(k.size() == 3);
    CHECK(k[0].component == &bg);
    CHECK(k[1].component == &content);
    CHECK(k[2].component == &overlay);

    // ml::Layer values are spaced so the underlying ints match.
    CHECK(k[0].layer == 0);
    CHECK(k[1].layer == 100);
    CHECK(k[2].layer == 200);
}

void test_user_enum_class_compiles_and_orders()
{
    enum class MyLayer : int
    {
        Backdrop = 10,
        Body     = 50,
        Modal    = 999,
    };

    Probe p;
    Leaf a, b, c;

    p.addComponent(c, MyLayer::Modal);
    p.addComponent(a, MyLayer::Backdrop);
    p.addComponent(b, MyLayer::Body);

    const auto& k = p.getChildren();
    CHECK(k[0].component == &a && k[0].layer == 10);
    CHECK(k[1].component == &b && k[1].layer == 50);
    CHECK(k[2].component == &c && k[2].layer == 999);
}

void test_addComponents_variadic_with_enum()
{
    Probe p;
    Leaf input, sendBtn, bg;

    p.addComponent(bg, ml::Layer::Background);
    p.addComponents(ml::Layer::Overlay, input, sendBtn);

    const auto& k = p.getChildren();
    CHECK(k.size() == 3);
    CHECK(k[0].component == &bg);
    CHECK(k[1].component == &input   && k[1].layer == 200);
    CHECK(k[2].component == &sendBtn && k[2].layer == 200);
}

void test_int_and_enum_paths_agree()
{
    // The enum overload is sugar over the int overload — they should produce
    // identical orderings.
    Probe pA;
    Leaf a1, a2, a3;
    pA.addComponent(a3, 200);
    pA.addComponent(a1,   0);
    pA.addComponent(a2, 100);

    Probe pB;
    Leaf b1, b2, b3;
    pB.addComponent(b3, ml::Layer::Overlay);
    pB.addComponent(b1, ml::Layer::Background);
    pB.addComponent(b2, ml::Layer::Content);

    const auto& kA = pA.getChildren();
    const auto& kB = pB.getChildren();
    for (size_t i = 0; i < kA.size(); ++i)
        CHECK(kA[i].layer == kB[i].layer);
}

} // namespace

int main()
{
    test_explicit_layer_orders_ascending();
    test_within_layer_keeps_insertion_order();
    test_default_layer_is_100();
    test_no_arg_addComponent_still_orders_by_insertion();
    test_readding_same_child_updates_layer();
    test_addComponents_variadic_default_layer();
    test_addComponents_variadic_shared_layer();
    test_framework_ml_layer_enum();
    test_user_enum_class_compiles_and_orders();
    test_addComponents_variadic_with_enum();
    test_int_and_enum_paths_agree();

    if (failures == 0) {
        std::cout << "ChildOrdering: all checks passed\n";
        return 0;
    }
    std::cerr << "ChildOrdering: " << failures << " check(s) failed\n";
    return 1;
}
