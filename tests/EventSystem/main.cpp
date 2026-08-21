// Event-system invariants — Subscribable subscribe/publish delivery, the
// publish-time filter, unsubscribeAll teardown, and AppManager's exclusive-owner
// registry (the gate the click filter uses). Window-free.

#include <Malena/Core/Component.h>
#include <Malena/Engine/App/AppManager.h>
#include <iostream>

namespace {

enum class Ev { Ping };

struct Node : public ml::Component<>
{
    int pings = 0;
    void draw(sf::RenderTarget&, sf::RenderStates) const override {}
};

int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
        ++failures; \
    } \
} while (0)

// ── subscribe + publish delivers to subscribers ──────────────────────────────
void test_publish_delivers()
{
    Node a;
    a.subscribe(Ev::Ping, [&]{ a.pings++; });

    a.publish(Ev::Ping);
    CHECK(a.pings == 1);
    a.publish(Ev::Ping);
    CHECK(a.pings == 2);

    a.unsubscribeAll();
}

// ── publish reaches every subscriber of the event, not just the publisher ────
void test_publish_multi_subscriber()
{
    Node a, b;
    a.subscribe(Ev::Ping, [&]{ a.pings++; });
    b.subscribe(Ev::Ping, [&]{ b.pings++; });

    a.publish(Ev::Ping);
    CHECK(a.pings == 1);
    CHECK(b.pings == 1);   // both subscribers receive it

    a.unsubscribeAll();
    b.unsubscribeAll();
}

// ── unsubscribeAll stops delivery ────────────────────────────────────────────
void test_unsubscribe_all()
{
    Node a, b;
    a.subscribe(Ev::Ping, [&]{ a.pings++; });
    b.subscribe(Ev::Ping, [&]{ b.pings++; });

    a.unsubscribeAll();
    b.publish(Ev::Ping);
    CHECK(a.pings == 0);   // a no longer subscribed
    CHECK(b.pings == 1);

    b.unsubscribeAll();
}

// ── a publish-time filter that rejects suppresses the whole fire ─────────────
void test_publish_filter()
{
    Node a;
    a.subscribe(Ev::Ping, [&]{ a.pings++; });

    a.publish(Ev::Ping, [](ml::EventReceiver&){ return false; });
    CHECK(a.pings == 0);   // filter rejected → nothing fired

    a.publish(Ev::Ping, [](ml::EventReceiver&){ return true; });
    CHECK(a.pings == 1);

    a.unsubscribeAll();
}

// ── AppManager exclusive-owner registry ──────────────────────────────────────
void test_exclusive_owner_registry()
{
    Node a, b;
    ml::AppManager::clearExclusiveOwner();
    CHECK(ml::AppManager::exclusiveOwner() == nullptr);
    CHECK(ml::AppManager::isUnderExclusiveOwner(&a));   // no owner ⇒ everyone passes

    ml::AppManager::setExclusiveOwner(&a);
    CHECK(ml::AppManager::isUnderExclusiveOwner(&a));    // owner itself
    CHECK(!ml::AppManager::isUnderExclusiveOwner(&b));   // unrelated node locked out

    ml::AppManager::clearExclusiveOwner();
    CHECK(ml::AppManager::isUnderExclusiveOwner(&b));    // released
}

} // namespace

int main()
{
    test_publish_delivers();
    test_publish_multi_subscriber();
    test_unsubscribe_all();
    test_publish_filter();
    test_exclusive_owner_registry();

    if (failures == 0) { std::cout << "EventSystem: all checks passed\n"; return 0; }
    std::cerr << "EventSystem: " << failures << " check(s) failed\n";
    return 1;
}
