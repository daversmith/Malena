#include <Malena/Engine/App/Application.h>
#include <Malena/Graphics/Controls/RectangleButton.h>
#include <Malena/Graphics/Controls/ScrollPane.h>      // the REWORKED real control
#include <Malena/Engine/Events/Fireable.h>            // synthesize a real click for the proof
#include <Malena/Engine/Window/WindowManager.h>

#include <SFML/Graphics/Texture.hpp>
#include <array>
#include <cstdio>
#include <string>

// Validation for the reworked ml::ScrollPane (viewport-clip, no RenderTexture,
// interactive children). 9 clickable rows in a pane that fits ~5; a status banner
// + "+ Add option" stay OUTSIDE the pane. The scripted sequence self-captures at
// top / scrolled / after a synthesized click on a row only reachable by scrolling.
class ScrollPaneValidate : public ml::Application
{
    static constexpr int   kRows  = 9;
    static constexpr float kRowH  = 40.f;
    static constexpr float kPaneX = 40.f, kPaneY = 70.f, kPaneW = 360.f, kPaneH = 200.f;

    ml::ScrollPane                           _pane{ kPaneW, kPaneH };
    std::array<ml::RectangleButton, kRows>   _rows;
    ml::RectangleButton                      _addBtn;
    ml::RectangleButton                      _status;
    int _frame = 0;

public:
    ScrollPaneValidate() : ml::Application(440, 360, 32, "ml::ScrollPane (reworked)") {}

    void onInit() override
    {
        _status.setSize({ kPaneW, 28.f });
        _status.setPosition({ kPaneX, 30.f });
        _status.setString("Click a row  (scroll to reach hidden ones)");
        _status.setCharacterSize(14);
        _status.setFillColor(sf::Color(50, 54, 62));
        _status.setTextColor(sf::Color::White);
        addComponent(_status);

        _pane.setPosition({ kPaneX, kPaneY });
        _pane.setBackgroundColor(sf::Color(30, 32, 38));
        addComponent(_pane);

        for (int i = 0; i < kRows; ++i)
        {
            auto& b = _rows[i];
            b.setSize({ kPaneW - 12.f, kRowH });          // ScrollPane stacks by height
            b.setString("Option " + std::to_string(i + 1));
            b.setCharacterSize(15);
            b.setFillColor(sf::Color(64, 70, 82));
            b.setTextColor(sf::Color::White);
            b.onClick([this, i] {
                _status.setString("Clicked: Option " + std::to_string(i + 1));
                for (auto& r : _rows) r.setFillColor(sf::Color(64, 70, 82));
                _rows[i].setFillColor(sf::Color(60, 160, 90));
            });
            _pane.addComponent(b);
        }

        _addBtn.setSize({ 130.f, 28.f });
        _addBtn.setPosition({ kPaneX, kPaneY + kPaneH + 12.f });
        _addBtn.setString("+ Add option");
        _addBtn.setCharacterSize(13);
        _addBtn.setFillColor(sf::Color(50, 90, 160));
        _addBtn.setTextColor(sf::Color::White);
        addComponent(_addBtn);

        // Interactive: wheel-scroll the list, click rows. (Re-enable the line
        // below to regenerate the scripted /tmp/sp_*.png self-capture proof.)
        // onPostRender([this] { script(); });
    }

private:
    void save(const std::string& path)
    {
        sf::RenderWindow& w = ml::WindowManager::getWindow();
        sf::Texture tex(w.getSize());
        tex.update(w);
        (void)tex.copyToImage().saveToFile(path);
    }

    void moveCursor(sf::Vector2f p)
    {
        sf::Mouse::setPosition({ static_cast<int>(p.x), static_cast<int>(p.y) },
                               ml::WindowManager::getWindow());
    }

    void clickHere()
    {
        sf::RenderWindow& w = ml::WindowManager::getWindow();
        const sf::Vector2i px = sf::Mouse::getPosition(w);
        std::fprintf(stderr, "[click] cursor=(%d,%d)\n", px.x, px.y);
        sf::Event::MouseButtonReleased e;
        e.button = sf::Mouse::Button::Left;
        e.position = px;
        std::optional<sf::Event> ev = sf::Event(e);
        int fired = 0;
        for (auto* d : ml::Fireable::_fireables)
            if (d && d->occurred(ev)) { d->fire(ev); ++fired; }
        std::fprintf(stderr, "[click] dispatchers fired=%d\n", fired);
    }

    sf::Vector2f rowCenter(int i) const
    {
        const float y = kPaneY + i * kRowH - _pane.getScrollOffsetY() + kRowH / 2.f;
        return { kPaneX + (kPaneW - 12.f) / 2.f, y };
    }

    void script()
    {
        ++_frame;
        switch (_frame)
        {
            case 20: save("/tmp/sp_1_top.png");               break;
            case 30: _pane.setScrollOffsetY(10000.f);          break;  // clamp → bottom
            case 40: save("/tmp/sp_2_scrolled.png");          break;
            case 48: moveCursor(rowCenter(8));                 break;  // park cursor on Option 9
            case 54: clickHere();                              break;  // dispatch release
            case 65: save("/tmp/sp_3_clicked.png");           break;
            case 75: ml::WindowManager::getWindow().close();   break;
            default: break;
        }
    }
};

int main()
{
    ScrollPaneValidate app;
    app.run();
    return 0;
}
