// Manifest + resource invariants — the static enum→value registry every
// component's manifest uses, and the default-font singleton controls fall back
// on. Window-free (no GL needed to read a registry or the default font handle).

#include <Malena/Manifests/Manifest.h>
#include <Malena/Resources/ConfigManager.h>
#include <Malena/Resources/FontManager.h>
#include <string>
#include <iostream>

namespace {

int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
        ++failures; \
    } \
} while (0)

// A manifest populated in its static initializer, as real manifests are.
class TestManifest : public ml::Manifest
{
public:
    enum class Images   { Background, Player };
    enum class Text     { Title };
    enum class Ints     { MaxPlayers };
    enum class Floats   { Scale };
    enum class Booleans { ShowHUD };

    inline static const auto _ = [](){
        set(Images::Background, "assets/bg.png");
        set(Images::Player,     "assets/player.png");
        set(Text::Title,        std::string("My App"));
        set(Ints::MaxPlayers,   4);
        set(Floats::Scale,      1.5f);
        set(Booleans::ShowHUD,  true);
        return 0;
    }();
};

// ── Asset paths round-trip by enum key ───────────────────────────────────────
void test_manifest_filepaths()
{
    CHECK(TestManifest::getFilepath(TestManifest::Images::Background) == "assets/bg.png");
    CHECK(TestManifest::getFilepath(TestManifest::Images::Player)     == "assets/player.png");
    // Different enum keys address independent slots.
    CHECK(TestManifest::getFilepath(TestManifest::Images::Background)
          != TestManifest::getFilepath(TestManifest::Images::Player));
}

// ── Typed config values round-trip ───────────────────────────────────────────
void test_manifest_config()
{
    const int maxPlayers =
        TestManifest::getConfig<TestManifest::Ints, int>(TestManifest::Ints::MaxPlayers);
    CHECK(maxPlayers == 4);
}

// ── ConfigManager::get for every config type (regression for the typo bugs) ──
// ConfigManager::get has one overload per config enum (Text/Ints/Floats/Booleans).
// Int retrieval used to take `M::Integers` (never matched the documented `Ints`),
// and bool's SFINAE trait tested `T::Floats` instead of `T::Booleans` — both made
// those overloads impossible to instantiate. This pins all four as callable.
void test_config_manager_all_types()
{
    using Cfg = ml::ConfigManager<TestManifest>;
    CHECK(Cfg::get(TestManifest::Text::Title)       == "My App");
    CHECK(Cfg::get(TestManifest::Ints::MaxPlayers)  == 4);          // was uncallable
    CHECK(Cfg::get(TestManifest::Floats::Scale)     == 1.5f);
    CHECK(Cfg::get(TestManifest::Booleans::ShowHUD) == true);        // was uncallable
}

// ── FontManager default is a stable singleton ────────────────────────────────
// Controls call FontManager<>::getDefault() in their default ctor; every caller
// must get the same font instance (not a fresh copy per call).
void test_default_font_singleton()
{
    const sf::Font& a = ml::FontManager<>::getDefault();
    const sf::Font& b = ml::FontManager<>::getDefault();
    CHECK(&a == &b);
}

} // namespace

int main()
{
    test_manifest_filepaths();
    test_manifest_config();
    test_config_manager_all_types();
    test_default_font_singleton();

    if (failures == 0) { std::cout << "ManifestResources: all checks passed\n"; return 0; }
    std::cerr << "ManifestResources: " << failures << " check(s) failed\n";
    return 1;
}
