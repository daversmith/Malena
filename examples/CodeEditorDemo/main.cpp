#include <Malena/Engine/App/Application.h>
#include <Malena/Graphics/Text/CodeEditor.h>

class CodeEditorDemo : public ml::Application
{
    ml::CodeEditor _editor;
public:
    CodeEditorDemo() : ml::Application(900, 600, 32, "Malena CodeEditor Demo") {}

    void onInit() override
    {
        _editor.setPosition({ 20.f, 20.f });
        _editor.setSize({ 860.f, 560.f });
        _editor.setLanguage(ml::CodeLanguage::Cpp);
        _editor.setValue(
            "// Malena CodeEditor — syntax color-coding (not a linter)\n"
            "#include <string>\n"
            "\n"
            "int add(int a, int b) {\n"
            "    // return the sum\n"
            "    return a + b;   /* try editing me */\n"
            "}\n"
            "\n"
            "const char* msg = \"hello, world\";\n"
            "double pi = 3.14159;\n"
            "bool ready = true;\n");
        addComponent(_editor);
    }
};

int main()
{
    CodeEditorDemo app;
    app.run();
    return 0;
}
