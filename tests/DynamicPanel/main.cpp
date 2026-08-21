// Locks DynamicPanel's retained-rebuild contract: pooling (widgets are reused,
// never destroyed, leftovers hidden), keyed inputs (same physical field across
// rebuilds → focus/value survive), and the invalidate() → deferred-rebuild flag.
// Because widgets are pooled (never freed), a rebuild triggered from a widget's
// own handler is memory-safe — this pins the "same pointer across rebuilds"
// guarantee that underpins that. Window-free (uses the default-font singleton).

#include <Malena/Graphics/Layouts/DynamicPanel.h>
#include <iostream>
#include <string>

namespace {

// Re-export the protected rebuild + child accessor so the test can drive a
// synchronous rebuild (standing in for the framework's frame-boundary tick).
struct Probe : public ml::DynamicPanel
{
    using ml::DynamicPanel::doRebuild;
    using ml::Core::getChildren;
};

int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
        ++failures; \
    } \
} while (0)

using B = ml::DynamicPanel::Builder;

void test_pooling_reuses_and_hides_leftovers()
{
    Probe p;
    int n = 3;
    p.onBuild([&](B& b){ for (int i = 0; i < n; ++i) b.button("B", 0, i * 30.f, 100, 28, 0, []{}); });

    p.doRebuild();
    CHECK(p.getChildren().size() == 3);

    p.doRebuild();                                  // same count → no growth
    CHECK(p.getChildren().size() == 3);

    n = 5; p.doRebuild();                           // grow → pool grows
    CHECK(p.getChildren().size() == 5);

    n = 2; p.doRebuild();                           // shrink → pool RETAINED (not destroyed)
    CHECK(p.getChildren().size() == 5);
    int visible = 0;
    for (const auto& e : p.getChildren()) if (e.component->isVisible()) ++visible;
    CHECK(visible == 2);                            // only the 2 used are shown
}

void test_widget_pointers_stable_across_rebuild()
{
    Probe p;
    ml::RectangleButton* seen = nullptr;
    p.onBuild([&](B& b){ seen = &b.button("X", 0, 0, 100, 28, 0, []{}); });

    p.doRebuild();
    ml::RectangleButton* first = seen;
    p.doRebuild();                                  // pooled → the SAME object is reused
    CHECK(seen == first);                           // no destruction/realloc ⇒ no UAF on rebuild-from-handler
}

void test_input_keyed_stable_and_seeded()
{
    Probe p;
    ml::TextInput* seen = nullptr;
    std::string model = "hi";
    p.onBuild([&](B& b){ seen = &b.input("msg", "ph", 0, 0, 200, 28, model, [&](const std::string& v){ model = v; }); });

    p.doRebuild();
    ml::TextInput* first = seen;
    CHECK(first->getValue() == "hi");

    model = "bye";
    p.doRebuild();
    CHECK(seen == first);                           // same physical input across rebuilds
    CHECK(first->getValue() == "bye");              // re-seeded (not focused)
}

void test_unused_input_is_parked()
{
    Probe p;
    bool includeA = true;
    ml::TextInput* a = nullptr;
    p.onBuild([&](B& b){ if (includeA) a = &b.input("a", "ph", 0, 0, 100, 28, "", nullptr); });

    p.doRebuild();
    CHECK(a && a->isVisible());
    includeA = false;
    p.doRebuild();                                  // "a" no longer built → parked
    CHECK(!a->isVisible());
}

void test_invalidate_sets_pending_flag()
{
    Probe p;
    int builds = 0;
    p.onBuild([&](B& b){ ++builds; b.text("hi", 0, 0, 12, sf::Color::White); });
    CHECK(p.isRebuildPending());                    // onBuild marks dirty
    p.doRebuild();
    CHECK(!p.isRebuildPending());
    CHECK(builds == 1);

    p.invalidate();
    CHECK(p.isRebuildPending());                    // invalidate re-arms (safe to call from a handler)
    p.doRebuild();
    CHECK(!p.isRebuildPending());
    CHECK(builds == 2);
}

} // namespace

int main()
{
    test_pooling_reuses_and_hides_leftovers();
    test_widget_pointers_stable_across_rebuild();
    test_input_keyed_stable_and_seeded();
    test_unused_input_is_parked();
    test_invalidate_sets_pending_flag();

    if (failures == 0) { std::cout << "DynamicPanel: all checks passed\n"; return 0; }
    std::cerr << "DynamicPanel: " << failures << " check(s) failed\n";
    return 1;
}
