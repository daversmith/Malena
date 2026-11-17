// API-consistency regressions — the "a public knob that silently does nothing"
// class, plus the shape of the *With wrappers.
//
//   * Every *With<Manifest> exposes its base's constructors (using Base::Base).
//   * Applications receive keyboard events (Keyable), like every Core does.
//   * TextInput's theme tokens reach the drawing (GitHub #36 text color,
//     #37 corner radius) and so do setFont / setCharacterSize.
//   * centerHorizontally is the spelling; centerHorizonally still compiles.
//
// Window-free.

#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Graphics/Primitives/Circle.h>
#include <Malena/Graphics/Primitives/Convex.h>
#include <Malena/Graphics/Sprites/Sprite.h>
#include <Malena/Graphics/Controls/RectangleButton.h>
#include <Malena/Graphics/Controls/CircleButton.h>
#include <Malena/Graphics/Controls/ConvexButton.h>
#include <Malena/Graphics/Text/TextInput.h>
#include <Malena/Graphics/Text/TextArea.h>
#include <Malena/Engine/App/Application.h>
#include <Malena/Manifests/Manifest.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <iostream>
#include <type_traits>

namespace {

int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
        ++failures; \
    } \
} while (0)

struct M : ml::Manifest {};

// ── *With inherits the base constructors ─────────────────────────────────────
static_assert(std::is_constructible_v<ml::CircleWith<M>, sf::CircleShape>,
              "CircleWith must inherit Graphic(const U&)");
static_assert(std::is_constructible_v<ml::RectangleButtonWith<M>,
                                      const sf::Font&, std::optional<sf::Vector2f>,
                                      const std::string&, unsigned int>,
              "RectangleButtonWith must inherit Button's constructor");
static_assert(std::is_constructible_v<ml::CircleButtonWith<M>, const sf::Font&>);
static_assert(std::is_constructible_v<ml::ConvexButtonWith<M>, const sf::Font&>);
static_assert(std::is_default_constructible_v<ml::RectangleWith<M>>);
static_assert(std::is_default_constructible_v<ml::ConvexWith<M>>);
static_assert(std::is_default_constructible_v<ml::SpriteWith<M>>);

// ── Applications can listen for keys ─────────────────────────────────────────
static_assert(std::is_base_of_v<ml::Keyable, ml::Application>);
static_assert(std::is_base_of_v<ml::Keyable, ml::ApplicationWith<M>>);

// Peek at what TextInput will actually draw.
struct ProbeInput : ml::TextInput
{
    sf::Color drawnTextColor() const { return _renderer.getDefaultColor(); }
    unsigned  drawnTextSize()  const { return _renderer.getDefaultSize(); }
    float     drawnRadius()    const { return _background.getRadius(); }
};

void test_textinput_text_color()   // #36
{
    ProbeInput in;
    in.setTextColor(sf::Color::Black);
    CHECK(in.getTextColor() == sf::Color::Black);
    CHECK(in.drawnTextColor() == sf::Color::Black);

    // applyTheme (assigns the struct) must reach the drawing too.
    ml::TextInputTheme t = static_cast<const ml::TextInputTheme&>(in);
    t.textColor = sf::Color::Red;
    in.applyTheme(t);
    CHECK(in.drawnTextColor() == sf::Color::Red);

    // Disabled uses disabledTextColor; re-enabling restores textColor.
    in.setEnabled(false);
    CHECK(in.drawnTextColor() == in.getDisabledTextColor());
    in.setEnabled(true);
    CHECK(in.drawnTextColor() == sf::Color::Red);
}

void test_textinput_radius()       // #37
{
    ProbeInput in;
    in.setRadius(12.f);
    CHECK(in.getRadius() == 12.f);
    CHECK(in.drawnRadius() == 12.f);

    ml::TextInputTheme t = static_cast<const ml::TextInputTheme&>(in);
    t.radius = 3.f;
    in.applyTheme(t);
    CHECK(in.drawnRadius() == 3.f);
}

void test_textinput_char_size()
{
    ProbeInput in;
    in.setCharacterSize(27);
    CHECK(in.drawnTextSize() == 27u);
}

void test_textarea_text_color()
{
    struct ProbeArea : ml::TextArea
    { sf::Color drawn() const { return _renderer.getDefaultColor(); } } area;
    area.setTextColor(sf::Color::Blue);
    CHECK(area.drawn() == sf::Color::Blue);
}

void test_center_horizontally()
{
    ml::Rectangle parent, child;
    parent.setSize({200.f, 100.f});
    parent.setPosition({0.f, 0.f});
    child.setSize({50.f, 10.f});
    child.centerHorizontally(parent);
    CHECK(child.getPosition().x == 75.f);
}

} // namespace

int main()
{
    test_textinput_text_color();
    test_textinput_radius();
    test_textinput_char_size();
    test_textarea_text_color();
    test_center_horizontally();

    if (failures) { std::cerr << failures << " failure(s)\n"; return 1; }
    std::cout << "ApiConsistency: all passed\n";
    return 0;
}
