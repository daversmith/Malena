#include <Malena/Engine/App/Application.h>
#include <Malena/Graphics/Controls/ColorChooser.h>
#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Graphics/Text/Text.h>

class ColorPickerDemo : public ml::Application
{
    ml::Text         _label;
    ml::ColorChooser _chooser;
    ml::Rectangle    _preview;
public:
    ColorPickerDemo() : ml::Application(420, 420, 32, "Malena ColorChooser") {}

    void onInit() override
    {
        _label.setString("Color / Swatches tabs:");
        _label.setCharacterSize(16);
        _label.setPosition({ 30.f, 18.f });

        _chooser.setPosition({ 30.f, 50.f });
        _chooser.setSize({ 280.f, 230.f });

        _preview.setSize({ 280.f, 80.f });
        _preview.setPosition({ 30.f, 300.f });
        _preview.setFillColor(sf::Color(200, 200, 200));

        _chooser.onColorSelected([this](const sf::Color& c) { _preview.setFillColor(c); });

        addComponent(_label);
        addComponent(_chooser);
        addComponent(_preview);
    }
};

int main()
{
    ColorPickerDemo app;
    app.run();
    return 0;
}
