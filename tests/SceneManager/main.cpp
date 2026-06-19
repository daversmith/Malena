// SceneManager invariants — the instance-based scene router that reacts to an
// owner's setState(): activate adds/enables a scene, deactivate removes/disables
// the old one and pushes history, back() returns to the prior scene, and lazy
// scenes construct on first visit. Window-free.

#include <Malena/Core/Component.h>
#include <Malena/Engine/Scene/SceneManager.h>
#include <iostream>

namespace {

int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
        ++failures; \
    } \
} while (0)

class OwnerManifest : public ml::Manifest
{
public:
    enum class State { A, B, C };
};

struct Owner : public ml::ComponentWith<OwnerManifest>
{
    using S = OwnerManifest::State;
    ml::SceneManager<S> scenes;
    void draw(sf::RenderTarget&, sf::RenderStates) const override {}
};

struct Scene : public ml::Component<>
{
    void draw(sf::RenderTarget&, sf::RenderStates) const override {}
};

// A lazily-constructed scene that records how many times it has been built.
struct LazyScene : public ml::Component<>
{
    static int ctorCount;
    LazyScene() { ++ctorCount; }
    void draw(sf::RenderTarget&, sf::RenderStates) const override {}
};
int LazyScene::ctorCount = 0;

// activate()/deactivate() set the scene's own enabled flag — the observable we
// use to tell which scene is currently mounted.
void test_start_and_navigate()
{
    Owner owner;
    Scene a, b;
    owner.scenes.bind(Owner::S::A, a);
    owner.scenes.bind(Owner::S::B, b);
    owner.scenes.attach(owner);

    owner.scenes.start(Owner::S::A);
    CHECK(owner.scenes.isActive(Owner::S::A));
    CHECK(a.checkFlag(ml::Flag::ENABLED));     // A mounted

    owner.setState(Owner::S::B);               // forward navigation
    CHECK(!a.checkFlag(ml::Flag::ENABLED));     // A deactivated
    CHECK(b.checkFlag(ml::Flag::ENABLED));      // B mounted
}

// back() pops history and returns to the previously-visited scene.
void test_history_back()
{
    Owner owner;
    Scene a, b;
    owner.scenes.bind(Owner::S::A, a);
    owner.scenes.bind(Owner::S::B, b);
    owner.scenes.attach(owner);

    owner.scenes.start(Owner::S::A);
    owner.setState(Owner::S::B);               // A pushed to history
    CHECK(b.checkFlag(ml::Flag::ENABLED));

    owner.scenes.back();                        // → A
    CHECK(owner.isState(Owner::S::A));
    CHECK(a.checkFlag(ml::Flag::ENABLED));
    CHECK(!b.checkFlag(ml::Flag::ENABLED));
}

// Lazy scenes are constructed on first visit, not at bind time.
void test_lazy_construction()
{
    LazyScene::ctorCount = 0;
    Owner owner;
    Scene a;
    owner.scenes.bind(Owner::S::A, a);
    owner.scenes.bindLazy<LazyScene>(Owner::S::C);
    owner.scenes.attach(owner);
    owner.scenes.start(Owner::S::A);

    CHECK(owner.scenes.has(Owner::S::C));
    CHECK(LazyScene::ctorCount == 0);          // not built until visited

    owner.setState(Owner::S::C);
    CHECK(LazyScene::ctorCount == 1);          // constructed on first visit
}

} // namespace

int main()
{
    test_start_and_navigate();
    test_history_back();
    test_lazy_construction();

    if (failures == 0) { std::cout << "SceneManager: all checks passed\n"; return 0; }
    std::cerr << "SceneManager: " << failures << " check(s) failed\n";
    return 1;
}
