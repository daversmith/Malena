#include <Malena/Engine/App/Application.h>
#include <Malena/Graphics/Controls/InfoButton.h>
#include <Malena/Graphics/Text/Text.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <SFML/Graphics/Texture.hpp>

// Validates ml::InfoButton: a badge + a help popover (opened programmatically here).
class InfoButtonDemo : public ml::Application
{
    ml::Text       _label;
    ml::InfoButton _info;
    int _frame = 0;
public:
    InfoButtonDemo() : ml::Application(420, 240, 32, "ml::InfoButton") {}

    void onInit() override
    {
        _label.setString("Fill-in-the-blank sentence");
        _label.setCharacterSize(16);
        _label.setPosition({ 30.f, 40.f });
        addComponent(_label);

        _info.setText("Mark each blank as {{answer}} and list extra accepted answers "
                      "with |, e.g. {{wall|brick wall}}. Students see a numbered blank "
                      "and type their answer.");
        _info.setPopoverWidth(320.f);
        _info.setPosition({ 250.f, 40.f });
        addComponent(_info);

        onPostRender([this] {
            if (++_frame == 15) _info.open();
            if (_frame == 25) {
                auto& w = ml::WindowManager::getWindow();
                sf::Texture t(w.getSize()); t.update(w);
                (void)t.copyToImage().saveToFile("/tmp/infobtn.png");
            }
            if (_frame >= 30) ml::WindowManager::getWindow().close();
        });
    }
};

int main() { InfoButtonDemo app; app.run(); return 0; }
