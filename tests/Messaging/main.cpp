// Messaging invariants — the typed MessageManager bus (distinct from the event
// system): subscribers keyed by opaque pointer, messages carry typed payloads,
// and channels are separated by (enum value, DataType). Window-free.

#include <Malena/Engine/Messaging/MessageManager.h>
#include <string>
#include <iostream>

namespace {

enum class Msg { Score, Name };

int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
        ++failures; \
    } \
} while (0)

// ── Typed deliver + unsubscribe ──────────────────────────────────────────────
void test_typed_delivery()
{
    int tokenA;          // opaque subscriber identity
    int got = 0;
    ml::MessageManager::subscribe<int>(Msg::Score, &tokenA,
        [&](const int& v){ got = v; });

    ml::MessageManager::publish<int>(Msg::Score, 42);
    CHECK(got == 42);
    ml::MessageManager::publish<int>(Msg::Score, 7);
    CHECK(got == 7);

    ml::MessageManager::unsubscribe<int>(Msg::Score, &tokenA);
    ml::MessageManager::publish<int>(Msg::Score, 99);
    CHECK(got == 7);     // unsubscribed → no longer updated
}

// ── Multiple subscribers all receive the same message ────────────────────────
void test_multi_subscriber()
{
    int t1, t2;
    int a = 0, b = 0;
    ml::MessageManager::subscribe<int>(Msg::Score, &t1, [&](const int& v){ a = v; });
    ml::MessageManager::subscribe<int>(Msg::Score, &t2, [&](const int& v){ b = v; });

    ml::MessageManager::publish<int>(Msg::Score, 5);
    CHECK(a == 5);
    CHECK(b == 5);

    ml::MessageManager::unsubscribe<int>(Msg::Score, &t1);
    ml::MessageManager::unsubscribe<int>(Msg::Score, &t2);
}

// ── Channels are separated by enum value AND payload type ────────────────────
void test_channel_separation()
{
    int tokenScore, tokenName;
    int scoreHits = 0;
    std::string lastName;

    ml::MessageManager::subscribe<int>(Msg::Score, &tokenScore,
        [&](const int&){ scoreHits++; });
    ml::MessageManager::subscribe<std::string>(Msg::Name, &tokenName,
        [&](const std::string& s){ lastName = s; });

    ml::MessageManager::publish<std::string>(Msg::Name, "alice");
    CHECK(lastName == "alice");
    CHECK(scoreHits == 0);          // a Name message must not hit the Score channel

    ml::MessageManager::publish<int>(Msg::Score, 1);
    CHECK(scoreHits == 1);
    CHECK(lastName == "alice");     // unchanged

    ml::MessageManager::unsubscribe<int>(Msg::Score, &tokenScore);
    ml::MessageManager::unsubscribe<std::string>(Msg::Name, &tokenName);
}

} // namespace

int main()
{
    test_typed_delivery();
    test_multi_subscriber();
    test_channel_separation();

    if (failures == 0) { std::cout << "Messaging: all checks passed\n"; return 0; }
    std::cerr << "Messaging: " << failures << " check(s) failed\n";
    return 1;
}
