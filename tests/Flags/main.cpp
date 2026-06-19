// Flag & State invariants — the framework's two boolean/enum state systems.
//
// Every Core carries FlagManager<ml::Flag> (the built-in flags the dispatchers
// read). ComponentWith<Manifest> additionally composes custom per-manifest
// Flag and State enums. This locks both APIs and that they're independent.

#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <iostream>

namespace {

// A manifest with its own Flag and State vocabulary (like a real control).
class TestManifest : public ml::Manifest
{
public:
    enum class Flag  { Selected, Active };
    enum class State { Idle, Running, Done };
};

struct Widget : public ml::ComponentWith<TestManifest>
{
    using Flag  = TestManifest::Flag;
    using State = TestManifest::State;
    void draw(sf::RenderTarget&, sf::RenderStates) const override {}
};

int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
        ++failures; \
    } \
} while (0)

// ── Built-in ml::Flag: the five-method API ──────────────────────────────────
void test_builtin_flag_api()
{
    Widget w;
    // ENABLED starts true (every Core is enabled by construction).
    CHECK(w.checkFlag(ml::Flag::ENABLED));

    w.enableFlag(ml::Flag::DRAGGABLE);
    CHECK(w.checkFlag(ml::Flag::DRAGGABLE));
    w.disableFlag(ml::Flag::DRAGGABLE);
    CHECK(!w.checkFlag(ml::Flag::DRAGGABLE));

    w.toggleFlag(ml::Flag::FOCUSED);
    CHECK(w.checkFlag(ml::Flag::FOCUSED));
    w.toggleFlag(ml::Flag::FOCUSED);
    CHECK(!w.checkFlag(ml::Flag::FOCUSED));

    w.setFlag(ml::Flag::HOVERED, true);
    CHECK(w.checkFlag(ml::Flag::HOVERED));
    w.setFlag(ml::Flag::HOVERED, false);
    CHECK(!w.checkFlag(ml::Flag::HOVERED));
}

// ── Custom manifest flags are a SEPARATE namespace from ml::Flag ─────────────
void test_custom_flags_independent()
{
    Widget w;
    CHECK(!w.checkFlag(Widget::Flag::Selected));
    w.enableFlag(Widget::Flag::Selected);
    CHECK(w.checkFlag(Widget::Flag::Selected));
    CHECK(!w.checkFlag(Widget::Flag::Active));   // sibling flag unaffected

    // Toggling a custom flag must not disturb the built-in ENABLED flag.
    CHECK(w.checkFlag(ml::Flag::ENABLED));
    w.toggleFlag(Widget::Flag::Active);
    CHECK(w.checkFlag(Widget::Flag::Active));
    CHECK(w.checkFlag(ml::Flag::ENABLED));       // still enabled
}

// ── Custom State enum: setState / getState round-trip ────────────────────────
void test_state_round_trip()
{
    Widget w;
    // isState(value) is disambiguated by its argument type (getState() with no
    // arg is ambiguous across a component's several state managers).
    w.setState(Widget::State::Running);
    CHECK(w.isState(Widget::State::Running));
    w.setState(Widget::State::Done);
    CHECK(w.isState(Widget::State::Done));
    CHECK(!w.isState(Widget::State::Idle));
}

} // namespace

int main()
{
    test_builtin_flag_api();
    test_custom_flags_independent();
    test_state_round_trip();

    if (failures == 0) { std::cout << "Flags: all checks passed\n"; return 0; }
    std::cerr << "Flags: " << failures << " check(s) failed\n";
    return 1;
}
