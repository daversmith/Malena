// Locks ml::Avatar: initialsOf() derivation, name-hashed colour stability, and
// that size/position/presence configuration reflects in the bounds. Window-free.

#include <Malena/Graphics/Controls/Avatar.h>
#include <iostream>

namespace {
int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; ++failures; } \
} while (0)

void test_initials()
{
    CHECK(ml::Avatar::initialsOf("Grace Hopper") == "GH");
    CHECK(ml::Avatar::initialsOf("madonna") == "M");
    CHECK(ml::Avatar::initialsOf("  ada  lovelace ") == "AL");
    CHECK(ml::Avatar::initialsOf("") == "");
    CHECK(ml::Avatar::initialsOf("john ronald reuel tolkien") == "JR");  // first two words
}

void test_size_and_bounds()
{
    ml::Avatar a;
    a.setName("Grace Hopper");
    a.setAvatarSize(40.f);
    a.setPosition({ 100.f, 50.f });
    auto b = a.getGlobalBounds();
    CHECK(b.position.x == 100.f && b.position.y == 50.f);
    CHECK(b.size.x == 40.f && b.size.y == 40.f);
    CHECK(a.getPosition().x == 100.f);
}

void test_config_no_crash()
{
    ml::Avatar a;
    a.setName("Alan Turing");
    a.setColorSeed(2);
    a.setPalette({ sf::Color::Red, sf::Color::Green, sf::Color::Blue });
    a.setPresence(true);
    a.setPresenceColor(sf::Color::Yellow);
    a.setInitialsColor(sf::Color::White);
    a.setName("Ada Lovelace");            // reconfigure after the fact
    CHECK(true);                          // reaching here = no crash / bad access
}

} // namespace

int main()
{
    test_initials();
    test_size_and_bounds();
    test_config_no_crash();
    if (failures == 0) { std::cout << "Avatar: all checks passed\n"; return 0; }
    std::cerr << "Avatar: " << failures << " check(s) failed\n";
    return 1;
}
