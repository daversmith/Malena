// Interactive dispatch smoke harness — verifies CLICK routing end-to-end with a
// real window + synthetic input, the layer headless unit tests can't reach
// (isHovered needs a live window). Drives the framework exactly as the main loop
// does: places the cursor (deterministically, via MouseEvents::setTestCursor),
// injects a synthetic mouse-release through AppManager::injectInput, and asserts
// which onClick handlers fired.
//
// This is the harness that would have caught the topmost-wins toolbar regression:
// scenario 3 below ("toolbar under a full-window overlay") fails the moment a
// dispatch change lets an empty-handler overlay swallow the toolbar's click.
//
// NOTE: needs a display (it opens an SFML window). Run manually, not in headless
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

// A full-window component that auto-subscribes to CLICK with the empty handler
// every ml::Component<> gets in its constructor — i.e. the "blocker" that broke
// topmost-wins. It has no real onClick of its own.
struct Overlay : public ml::Component<>
{
    sf::FloatRect bounds{ {0.f, 0.f}, {800.f, 600.f} };
    void draw(sf::RenderTarget&, sf::RenderStates) const override {}
    sf::FloatRect getGlobalBounds() const override { return bounds; }
};

struct Harness : public ml::Application
{
    ml::RectangleButton toolbarBtn;   // top-left, like an AppShell toolbar button
    ml::RectangleButton otherBtn;     // a separate, non-overlapping button
    Overlay             overlay;      // full-window empty-handler subscriber

    int toolbarHits = 0;
    int otherHits   = 0;

    Harness() : ml::Application(sf::VideoMode({800, 600}), "DispatchSmoke") {}

    void onInit() override
    {
        toolbarBtn.setSize({100.f, 40.f});
        toolbarBtn.setPosition({10.f, 10.f});
        toolbarBtn.setString("Tab");
        toolbarBtn.onClick([this]{ ++toolbarHits; });
        addComponent(toolbarBtn);

        otherBtn.setSize({100.f, 40.f});
        otherBtn.setPosition({400.f, 300.f});
        otherBtn.setString("Other");
        otherBtn.onClick([this]{ ++otherHits; });
        addComponent(otherBtn);
        // overlay is added per-scenario so the no-overlay tests run first
    }

    void reset() { toolbarHits = otherHits = 0; }

    // Place the cursor (deterministic) and inject a left-button release there.
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
    app.clickAt(60, 30);                 // centre of toolbarBtn (10,10,100,40)
    CHECK(app.toolbarHits == 1);
    CHECK(app.otherHits == 0);

    // ── 2. A click misses a button it's not over ─────────────────────────────
    app.reset();
    app.clickAt(450, 320);               // centre of otherBtn, not toolbarBtn
    CHECK(app.otherHits == 1);
    CHECK(app.toolbarHits == 0);

    // ── 3. THE REGRESSION GUARD: toolbar still works under a full-window overlay
    // The overlay is a front-most, empty-handler CLICK subscriber covering the
    // toolbar. The toolbar button must STILL receive its click. (topmost-wins let
    // the overlay swallow it — this check fails the instant that regresses.)
    app.addComponent(app.overlay);       // added last → painted in front
    app.reset();
    app.clickAt(60, 30);                 // over the toolbar button, beneath the overlay
    CHECK(app.toolbarHits == 1);         // <-- the toolbar regression detector

    ml::MouseEvents::setTestCursor(std::nullopt);   // restore live cursor

    if (failures == 0) { std::cout << "DispatchSmoke: all checks passed\n"; return 0; }
    std::cerr << "DispatchSmoke: " << failures << " check(s) failed\n";
    return 1;
}
