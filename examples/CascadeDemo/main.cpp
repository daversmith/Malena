// Interactive demo for the children-unification refactor (Phase 1).
//
// Exercises the public surfaces that changed:
//   - Panel routes children through Core::_children (commit 7d98d92).
//   - The "independent child disable round-trips through a parent toggle"
//     invariant — the load-bearing cleanup behind every Phase 1 commit.
//
// To run:
//   cmake -S . -B build -DMALENA_BUILD_TESTS=ON
//   cmake --build build --target CascadeDemo
//   ./build/tests/CascadeDemo
//
// What you should see and try:
//
//   1. Three coloured buttons (RED, GREEN, BLUE) inside a Panel. Each prints
//      to the console when clicked.
//
//   2. "Toggle Panel" → all three grey out and stop responding. Toggle
//      again → all three light back up.
//
//   3. "Toggle Red" disables ONLY red. Green/Blue still work.
//
//   4. THE LOAD-BEARING CHECK: toggle Red off, then toggle Panel off, then
//      Panel on. Green and Blue light back up; RED STAYS GREY. That is the
//      invariant the original ChatWindow cleanup fixed — calling setEnabled
//      on children (the old buggy override) would have force-enabled Red
//      when the parent came back on. With Core's cascade only touching
//      _parentEnabled, Red's _selfEnabled = false survives the round-trip.
//
//   5. "Reset everything" puts everything back on for another round.

#include <Malena/common.hpp>
#include <Malena/Graphics/Layouts/Panel.h>
#include <iostream>
#include <sstream>

namespace {

class CascadeDemo : public ml::Application
{
    ml::Panel           panel;
    ml::RectangleButton redBtn, greenBtn, blueBtn;

    ml::RectangleButton togglePanel, toggleRed, reset;
    ml::Text            statusText;
    std::stringstream   log;

public:
    CascadeDemo() : ml::Application(900, 540, 32, "Cascade Demo") {}

protected:
    void onInit() override
    {
        // Panel + three colour buttons.
        panel.setPosition({20.f, 20.f});
        panel.setSize({400.f, 280.f});
        panel.setFillColor(sf::Color(60, 60, 80));

        redBtn.setSize({360.f, 60.f});
        redBtn.setPosition({40.f, 50.f});
        redBtn.setFillColor(sf::Color::Red);
        redBtn.setString("RED");

        greenBtn.setSize({360.f, 60.f});
        greenBtn.setPosition({40.f, 130.f});
        greenBtn.setFillColor(sf::Color::Green);
        greenBtn.setString("GREEN");

        blueBtn.setSize({360.f, 60.f});
        blueBtn.setPosition({40.f, 210.f});
        blueBtn.setFillColor(sf::Color::Blue);
        blueBtn.setString("BLUE");

        panel.addComponent(redBtn);
        panel.addComponent(greenBtn);
        panel.addComponent(blueBtn);

        // Controls.
        togglePanel.setSize({400.f, 50.f});
        togglePanel.setPosition({470.f, 20.f});
        togglePanel.setFillColor(sf::Color(80, 80, 160));
        togglePanel.setString("Toggle Panel");

        toggleRed.setSize({400.f, 50.f});
        toggleRed.setPosition({470.f, 80.f});
        toggleRed.setFillColor(sf::Color(80, 80, 160));
        toggleRed.setString("Toggle Red (self)");

        reset.setSize({400.f, 50.f});
        reset.setPosition({470.f, 140.f});
        reset.setFillColor(sf::Color(160, 80, 80));
        reset.setString("Reset everything");

        statusText.setCharacterSize(14);
        statusText.setFillColor(sf::Color::White);
        statusText.setPosition({470.f, 210.f});
        statusText.setString("Click any button to test cascade...\n");

        addComponent(panel);
        addComponent(togglePanel);
        addComponent(toggleRed);
        addComponent(reset);
        addComponent(statusText);
    }

    void onReady() override
    {
        redBtn.onClick([this]   { logEvent("RED clicked"); });
        greenBtn.onClick([this] { logEvent("GREEN clicked"); });
        blueBtn.onClick([this]  { logEvent("BLUE clicked"); });

        togglePanel.onClick([this]{
            const bool nowEnabled = !panel.isEnabled();
            panel.setEnabled(nowEnabled);
            refreshColors();
            logEvent(std::string("Panel ") + (nowEnabled ? "ENABLED" : "DISABLED"));
        });

        toggleRed.onClick([this]{
            // This toggles _selfEnabled on red. The whole point of the cascade
            // fix is that flipping the panel later does NOT undo this.
            const bool nowEnabled = !redBtn.isEnabled();
            redBtn.setEnabled(nowEnabled);
            refreshColors();
            logEvent(std::string("Red ") + (nowEnabled ? "self-enabled" : "self-disabled"));
        });

        reset.onClick([this]{
            panel.setEnabled(true);
            redBtn.setEnabled(true);
            greenBtn.setEnabled(true);
            blueBtn.setEnabled(true);
            refreshColors();
            log.str("");
            log << "Everything reset.\n";
            statusText.setString(log.str());
            std::cout << "---reset---\n";
        });
    }

private:
    void logEvent(const std::string& msg)
    {
        std::cout << msg << "\n";
        log << msg << "\n";
        auto s = log.str();
        if (s.size() > 1200) s = s.substr(s.size() - 1200);
        statusText.setString(s);
    }

    void refreshColors()
    {
        auto colorFor = [](ml::RectangleButton& b, sf::Color base) {
            b.setFillColor(b.isEnabled()
                ? base
                : sf::Color(base.r / 4, base.g / 4, base.b / 4, 220));
        };
        colorFor(redBtn,   sf::Color::Red);
        colorFor(greenBtn, sf::Color::Green);
        colorFor(blueBtn,  sf::Color::Blue);
    }
};

} // namespace

int main()
{
    CascadeDemo app;
    app.run();
    return 0;
}
