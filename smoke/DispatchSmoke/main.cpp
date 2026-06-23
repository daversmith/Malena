// Interactive dispatch smoke harness — verifies CLICK routing end-to-end with a
// real window + synthetic input, the layer headless unit tests can't reach
// (isHovered needs a live window). Drives the framework exactly as the main loop
// does: places the cursor (deterministically, via MouseEvents::setTestCursor),
// injects a synthetic mouse-release through AppManager::injectInput, and asserts
// which onClick handlers fired / which component holds focus.
//
// These scenarios encode the DESIRED dispatch behaviour. On the current
// all-hovered model the click-through case intentionally fails — it is the spec
// a single-winner fix must satisfy without breaking the others.
//
// NOTE: needs a display (opens an SFML window). Run manually, not in headless
// CI. Built behind MALENA_BUILD_TESTS but NOT registered with ctest.

#include <Malena/Engine/App/Application.h>
#include <Malena/Graphics/Controls/RectangleButton.h>
#include <Malena/Utilities/MouseEvents.h>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <optional>

namespace {

int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
        ++failures; \
    } \
} while (0)

// A component that auto-subscribes to CLICK with the empty handler every
// ml::Component<> gets in its constructor, but has NO real onClick of its own.
// Stands in for a decorative overlay / full-window container (the "blocker" that
// broke topmost-wins) and, when small, for a focus-only widget like TextInput
// (which also has no real onClick — it relies on the auto-subscribe for focus).
struct Passive : public ml::Component<>
{
    sf::FloatRect bounds;   // settable; zero-size = never hovered, so inert
    explicit Passive(sf::FloatRect b = {}) : bounds(b) {}
    void draw(sf::RenderTarget&, sf::RenderStates) const override {}
    sf::FloatRect getGlobalBounds() const override { return bounds; }
};

struct Harness : public ml::Application
{
    ml::RectangleButton toolbarBtn;    // top-left, like an AppShell toolbar button
    ml::RectangleButton otherBtn;      // separate, non-overlapping
    ml::RectangleButton backBtn;       // overlaps frontBtn, painted behind
    ml::RectangleButton frontBtn;      // overlaps backBtn, painted in front
    Passive             focusProbe{ { {600.f, 100.f}, {80.f, 40.f} } };   // focus-only (no real onClick)
    Passive             overlay;       // full-window blocker — sized only for the overlay scenarios

    int toolbarHits = 0, otherHits = 0, backHits = 0, frontHits = 0;

    Harness() : ml::Application(sf::VideoMode({800, 600}), "DispatchSmoke") {}

    static void place(ml::RectangleButton& b, sf::Vector2f pos)
    {
        b.setSize({100.f, 40.f});
        b.setPosition(pos);
        b.setString("btn");
    }

    void onInit() override
    {
        place(toolbarBtn, {10.f, 10.f});   toolbarBtn.onClick([this]{ ++toolbarHits; });
        place(otherBtn,   {400.f, 300.f}); otherBtn.onClick([this]{ ++otherHits; });
        place(backBtn,    {200.f, 200.f}); backBtn.onClick([this]{ ++backHits; });
        place(frontBtn,   {200.f, 200.f}); frontBtn.onClick([this]{ ++frontHits; });

        addComponent(toolbarBtn);
        addComponent(otherBtn);
        addComponent(focusProbe);
        addComponent(backBtn);
        addComponent(frontBtn);            // after backBtn → painted in front
        addComponent(overlay);             // always present; inert until sized full-window
    }

    void reset() { toolbarHits = otherHits = backHits = frontHits = 0; }

    void clickAt(int x, int y)
    {
        ml::MouseEvents::setTestCursor(sf::Vector2i{ x, y });
        std::optional<sf::Event> ev{
            sf::Event::MouseButtonReleased{ sf::Mouse::Button::Left, { x, y } } };
        injectInput(ev);
    }
};

} // namespace

int main()
{
    Harness app;
    app.onInit();
    app.onReady();

    // ── 1. A click lands on the button under the cursor ──────────────────────
    app.reset();
    app.clickAt(60, 30);                 // centre of toolbarBtn
    CHECK(app.toolbarHits == 1);
    CHECK(app.otherHits == 0);

    // ── 2. A click misses a button it's not over ─────────────────────────────
    app.reset();
    app.clickAt(450, 320);               // centre of otherBtn
    CHECK(app.otherHits == 1);
    CHECK(app.toolbarHits == 0);

    // ── 3. Two overlapping REAL buttons → only the front-most fires ──────────
    // DESIRED single-winner behaviour. On the all-hovered model this FAILS
    // (both fire) — it's the click-through the fix must close.
    app.reset();
    app.clickAt(250, 220);               // centre of the overlapping front/back buttons
    CHECK(app.frontHits == 1);
    CHECK(app.backHits == 0);            // <-- click-through guard

    // ── 4. Focus: clicking a focus-only widget focuses it; clicking elsewhere
    //        blurs it. The fix must NOT break this (TextInput works this way).
    app.clickAt(640, 120);               // centre of focusProbe
    CHECK(app.focusProbe.checkFlag(ml::Flag::FOCUSED));
    app.clickAt(450, 320);               // click otherBtn → focusProbe should blur
    CHECK(!app.focusProbe.checkFlag(ml::Flag::FOCUSED));

    // ── 5. Toolbar still works under a full-window empty-handler overlay ─────
    // The regression that broke the admin app. Overlay wants no clicks, so it
    // must be transparent — the toolbar button receives its click.
    app.overlay.bounds = { {0.f, 0.f}, {800.f, 600.f} };   // cover everything
    app.reset();
    app.clickAt(60, 30);
    CHECK(app.toolbarHits == 1);         // <-- the admin-toolbar regression detector

    // ── 6. A DISABLED overlay must not block clicks behind it ────────────────
    app.overlay.setEnabled(false);
    app.reset();
    app.clickAt(60, 30);
    CHECK(app.toolbarHits == 1);
    app.overlay.setEnabled(true);
    app.overlay.bounds = {};             // back to inert

    ml::MouseEvents::setTestCursor(std::nullopt);   // restore live cursor

    if (failures == 0) { std::cout << "DispatchSmoke: all checks passed\n"; return 0; }
    std::cerr << "DispatchSmoke: " << failures << " check(s) failed\n";
    return 1;
}
