#include <Malena/Engine/App/Application.h>
#include <Malena/Graphics/Text/RichTextEditor.h>

class RichTextEditorDemo : public ml::Application
{
    ml::RichTextEditor _editor;
public:
    RichTextEditorDemo() : ml::Application(1080, 520, 32, "Malena RichTextEditor Demo") {}

    void onInit() override
    {
        _editor.setPosition({ 20.f, 20.f });
        _editor.setSize({ 780.f, 480.f });
        _editor.setValue(
            "Select some text, then click  B / I / U  or a color swatch.\n"
            "This is a TinyMCE-style rich text editor built on TextArea.\n"
            "Content round-trips losslessly as JSON spans (getRichText).");
        addComponent(_editor);
    }
};

int main()
{
    RichTextEditorDemo app;
    app.run();
    return 0;
}
