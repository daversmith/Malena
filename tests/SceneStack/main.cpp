// SceneStack invariants — the owning stack of layered scenes: only the top
// scene is enabled, opaque scenes hide what they cover, overlays keep it drawn,
// changes requested during event dispatch wait for the stack's next update and
// apply in request order, lifecycle hooks fire on each transition, and scenes
// are destroyed when popped. Window-free.

#include <Malena/Core/Component.h>
#include <Malena/Engine/Scene/SceneStack.h>
#include <Malena/Engine/Scene/SceneLifecycle.h>
#include <Malena/Traits/Lifecycle/Updatable.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace {

int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
        ++failures; \
    } \
} while (0)

// Records lifecycle hooks and its own destruction into a shared log.
struct Scene : public ml::Component<>, public ml::SceneLifecycle
{
    std::string name;
    std::vector<std::string>* log;
    ml::Component<> control;   // stands in for a button the scene added

    Scene(std::string n, std::vector<std::string>* l = nullptr) : name(std::move(n)), log(l)
    {
        addComponent(control);
    }
    ~Scene() override { note("delete"); }

    void note(const std::string& what) { if (log) log->push_back(name + ":" + what); }
    void onSceneEnter()  override { note("enter"); }
    void onScenePause()  override { note("pause"); }
    void onSceneResume() override { note("resume"); }
    void onSceneExit()   override { note("exit"); }
    void draw(sf::RenderTarget&, sf::RenderStates) const override {}
};

bool live(const ml::Core* c)  { return c->checkFlag(ml::Flag::ENABLED); }
bool shown(const ml::Core* c) { return c->isVisible(); }

// Outside event dispatch a push applies at once; the top scene is enabled.
void test_push_outside_dispatch_applies_now()
{
    ml::SceneStack stack;
    Scene& menu = stack.push<Scene>("menu");
    CHECK(stack.size() == 1);
    CHECK(stack.top() == &menu);
    CHECK(!stack.hasPendingChanges());
    CHECK(live(&menu) && shown(&menu));
}

// An opaque push disables and hides the scene underneath, including its
// controls; popping it brings both back and destroys the popped scene.
void test_opaque_cover_and_pop()
{
    std::vector<std::string> log;
    ml::SceneStack stack;
    Scene& menu = stack.push<Scene>("menu", &log);
    Scene& game = stack.push<Scene>("game", &log);

    CHECK(stack.top() == &game);
    CHECK(!live(&menu) && !shown(&menu));
    CHECK(!live(&menu.control));         // cascade: the covered scene's button is off
    CHECK(live(&game) && live(&game.control));

    stack.pop();
    CHECK(stack.size() == 1 && stack.top() == &menu);
    CHECK(live(&menu) && shown(&menu) && live(&menu.control));
    CHECK(log.back() == "menu:resume");
    CHECK(std::find(log.begin(), log.end(), "game:delete") != log.end());
}

// An overlay keeps the scene underneath drawn, but still disabled.
void test_overlay_keeps_below_drawn()
{
    ml::SceneStack stack;
    Scene& menu  = stack.push<Scene>("menu");
    Scene& game  = stack.push<Scene>("game");
    Scene& pause = stack.pushOverlay<Scene>("pause");

    CHECK(live(&pause) && shown(&pause));
    CHECK(!live(&game) && shown(&game));     // drawn under the overlay
    CHECK(!live(&menu) && !shown(&menu));    // game is opaque, so menu stays hidden

    stack.pop();
    CHECK(live(&game) && shown(&game));
}

// Hooks fire in order: enter, pause when covered, resume when uncovered, exit
// before delete. replace() doesn't resume and re-pause the scene underneath.
void test_lifecycle_order_and_replace()
{
    std::vector<std::string> log;
    ml::SceneStack stack;
    stack.push<Scene>("menu", &log);
    stack.push<Scene>("game", &log);
    stack.replace<Scene>("over", &log);
    stack.pop();

    const std::vector<std::string> expected = {
        "menu:enter",
        "menu:pause", "game:enter",
        "game:exit", "game:delete", "over:enter",
        "over:exit", "over:delete", "menu:resume",
    };
    CHECK(log == expected);
    CHECK(stack.size() == 1);
}

// A scene that pops itself from its own update handler is not destroyed while
// that handler runs; the pop applies on the stack's next update.
void test_pop_inside_dispatch_is_deferred()
{
    ml::UpdatableDispatcher updates;
    std::vector<std::string> log;
    ml::SceneStack stack;
    stack.push<Scene>("menu", &log);
    Scene& game = stack.push<Scene>("game", &log);

    bool aliveAfterPop = false;
    game.onUpdate([&]
    {
        stack.pop();
        aliveAfterPop = std::find(log.begin(), log.end(), "game:delete") == log.end();
    });

    updates.fire();   // game asks to be popped; the stack may apply this frame or next
    updates.fire();
    CHECK(aliveAfterPop);
    CHECK(stack.size() == 1);
    CHECK(std::find(log.begin(), log.end(), "game:delete") != log.end());
}

// Changes saved during dispatch apply in the order they were requested, so
// push-then-pop removes the new scene rather than the one that asked.
void test_saved_changes_keep_order()
{
    ml::UpdatableDispatcher updates;
    ml::SceneStack stack;
    Scene& menu = stack.push<Scene>("menu");

    bool once = false;
    menu.onUpdate([&]
    {
        if (once) return;
        once = true;
        stack.push<Scene>("dialog");
        stack.pop();
    });

    updates.fire();
    updates.fire();
    CHECK(stack.size() == 1);
    CHECK(stack.top() == &menu);
}

// Covered scenes stop updating; the top one keeps going.
void test_covered_scene_stops_updating()
{
    ml::UpdatableDispatcher updates;
    ml::SceneStack stack;
    Scene& game = stack.push<Scene>("game");
    int ticks = 0;
    game.onUpdate([&]{ ticks++; });

    updates.fire();
    CHECK(ticks == 1);

    stack.pushOverlay<Scene>("pause");
    updates.fire();
    CHECK(ticks == 1);    // frozen under the pause screen, though still drawn

    stack.pop();
    updates.fire();
    CHECK(ticks == 2);
}

// clear() removes everything requested before it; pushes after it survive.
void test_clear_then_push()
{
    ml::UpdatableDispatcher updates;
    ml::SceneStack stack;
    Scene& game = stack.push<Scene>("game");

    Scene* fresh = nullptr;
    game.onUpdate([&]
    {
        if (fresh) return;
        stack.clear();
        fresh = &stack.push<Scene>("menu");
    });

    updates.fire();
    updates.fire();
    CHECK(stack.size() == 1);
    CHECK(stack.top() == fresh);
}

// of() finds the stack for a live scene and for one whose push is waiting.
void test_of_finds_owner()
{
    ml::UpdatableDispatcher updates;
    ml::SceneStack a, b;
    Scene& inA = a.push<Scene>("a");
    Scene& inB = b.push<Scene>("b");
    CHECK(ml::SceneStack::of(inA) == &a);
    CHECK(ml::SceneStack::of(inB) == &b);

    Scene* waiting = nullptr;
    bool foundWaiting = false;
    inA.onUpdate([&]
    {
        if (waiting) return;
        waiting = &a.push<Scene>("next");
        foundWaiting = ml::SceneStack::of(*waiting) == &a;
    });
    updates.fire();
    CHECK(foundWaiting);

    Scene stray("stray");
    CHECK(ml::SceneStack::of(stray) == nullptr);
}

// Popping an empty stack does nothing; the destructor deletes what's left.
void test_edges_and_teardown()
{
    std::vector<std::string> log;
    {
        ml::SceneStack stack;
        stack.pop(3);
        CHECK(stack.empty());
        stack.push<Scene>("menu", &log);
        stack.push<Scene>("game", &log);
        stack.pop(5);
        CHECK(stack.empty());
        stack.push<Scene>("last", &log);
    }
    CHECK(log.back() == "last:delete");
}

} // namespace

int main()
{
    test_push_outside_dispatch_applies_now();
    test_opaque_cover_and_pop();
    test_overlay_keeps_below_drawn();
    test_lifecycle_order_and_replace();
    test_pop_inside_dispatch_is_deferred();
    test_saved_changes_keep_order();
    test_covered_scene_stops_updating();
    test_clear_then_push();
    test_of_finds_owner();
    test_edges_and_teardown();

    if (failures == 0) { std::cout << "SceneStack: all checks passed\n"; return 0; }
    std::cerr << "SceneStack: " << failures << " check(s) failed\n";
    return 1;
}
