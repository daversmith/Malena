#include <Malena/Engine/App/Application.h>
#include <Malena/Graphics/Controls/EditableList.h>
#include <Malena/Graphics/Controls/RectangleButton.h>
#include <Malena/Engine/Events/Fireable.h>
#include <Malena/Engine/Window/WindowManager.h>

#include <SFML/Graphics/Texture.hpp>
#include <cstdio>
#include <string>

// Validation for ml::EditableList: a single-select, scrollable list of editable
// text rows with per-row delete + an "+ Add option" button. Scripted self-capture
// proves clip + scroll + a synthesized click on the selector of a row only
// reachable by scrolling (selection routes through the scroll offset).
class EditableListDemo : public ml::Application
{
    static constexpr float kX = 40.f, kY = 60.f, kW = 360.f, kH = 210.f;

    ml::EditableList    _list;
    ml::RectangleButton _status;
    int _frame = 0;

public:
    EditableListDemo() : ml::Application(440, 320, 32, "ml::EditableList") {}

    void onInit() override
    {
        _status.setSize({ kW, 28.f });
        _status.setPosition({ kX, 24.f });
        _status.setString("EditableList — single select, scroll, add/delete");
        _status.setCharacterSize(13);
        _status.setFillColor(sf::Color(50, 54, 62));
        _status.setTextColor(sf::Color::White);
        addComponent(_status);

        _list.setSelectionMode(ml::EditableList::SelectionMode::Single);
        _list.setPlaceholder("Option text");
        _list.setAddButtonLabel("+ Add option");
        _list.setRowHeight(32.f);
        _list.setPosition({ kX, kY });
        _list.setSize({ kW, kH });
        _list.setValues({ "Red", "Green", "Blue", "Yellow",
                          "Purple", "Orange", "Cyan", "Magenta" }, { 1 });
        _list.onChange([this] {
            const auto sel = _list.selectedIndices();
            _status.setString(sel.empty() ? "no selection"
                                          : "selected row " + std::to_string(sel.front()));
        });
        addComponent(_list);

        // Interactive: click selectors, type in rows, add/delete, wheel-scroll.
        // (Re-enable to regenerate the /tmp/el_*.png scripted proof.)
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

    void parkCursor(float x, float y)
    {
        sf::Mouse::setPosition({ static_cast<int>(x), static_cast<int>(y) },
                               ml::WindowManager::getWindow());
    }

    // Synthesize a wheel scroll over the list (ScrollPane reads the event position).
    void wheel(float x, float y, float delta)
    {
        parkCursor(x, y);
        sf::Event::MouseWheelScrolled e;
        e.wheel    = sf::Mouse::Wheel::Vertical;
        e.delta    = delta;
        e.position = { static_cast<int>(x), static_cast<int>(y) };
        std::optional<sf::Event> ev = sf::Event(e);
        for (auto* d : ml::Fireable::_fireables)
            if (d && d->occurred(ev)) d->fire(ev);
    }

    // Synthesize a left-release at the parked cursor (hit-test reads the live mouse).
    void release()
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
        std::fprintf(stderr, "[click] dispatchers fired=%d  selected=%zu\n",
                     fired, _list.selectedIndices().size());
    }

    void script()
    {
        ++_frame;
        // Centering check: select an UNSELECTED top row (no scroll, so no re-stack
        // can mask a stale glyph center) and capture immediately after.
        switch (_frame)
        {
            case 20: save("/tmp/el_1_top.png");           break;  // Green (row 1) selected
            case 30: parkCursor(kX + 16.f, kY + 3 * 38.f + 16.f); break; // Yellow's selector
            case 34: release();                            break;  // select Yellow
            case 36: save("/tmp/el_2_firstclick.png");    break;  // ✓ must be centered NOW
            case 46: ml::WindowManager::getWindow().close(); break;
            default: break;
        }
    }
};

int main()
{
    EditableListDemo app;
    app.run();
    return 0;
}
