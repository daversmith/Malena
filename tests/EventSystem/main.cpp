// Event-system invariants — Subscribable subscribe/publish delivery, the
// publish-time filter, unsubscribeAll teardown, AppManager's exclusive-owner
// registry (the gate the click filter uses), and the update filter skipping
// disabled components. Window-free.

#include <Malena/Core/Component.h>
#include <Malena/Engine/App/AppManager.h>
#include <Malena/Traits/Lifecycle/Updatable.h>
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

// ── onUpdate skips disabled components, directly or through an ancestor ─────
void test_update_skips_disabled()
{
    ml::UpdatableDispatcher updates;
    Node parent, child, plain;
    parent.addComponent(child);

    int parentUpdates = 0, childUpdates = 0, plainUpdates = 0;
    parent.onUpdate([&]{ parentUpdates++; });
    child.onUpdate([&]{ childUpdates++; });
    plain.onUpdate([&]{ plainUpdates++; });

    updates.fire();
    CHECK(parentUpdates == 1);
    CHECK(childUpdates == 1);
    CHECK(plainUpdates == 1);

    parent.setEnabled(false);
    updates.fire();
    CHECK(parentUpdates == 1);   // disabled: skipped
    CHECK(childUpdates == 1);    // disabled ancestor: skipped
    CHECK(plainUpdates == 2);    // unrelated: still updates

    parent.setEnabled(true);
    updates.fire();
    CHECK(parentUpdates == 2);   // updates resume once re-enabled
    CHECK(childUpdates == 2);

    parent.unsubscribeAll();
    child.unsubscribeAll();
    plain.unsubscribeAll();
}

} // namespace

int main()
{
    test_publish_delivers();
    test_publish_multi_subscriber();
    test_unsubscribe_all();
    test_publish_filter();
    test_exclusive_owner_registry();
    test_update_skips_disabled();

    if (failures == 0) { std::cout << "EventSystem: all checks passed\n"; return 0; }
    std::cerr << "EventSystem: " << failures << " check(s) failed\n";
    return 1;
}
