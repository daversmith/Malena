// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#include <Malena/Graphics/Text/CodeEditor.h>
#include <Malena/Resources/FontManager.h>

#include <cctype>

namespace ml
{
    // =========================================================================
    // BasicSyntaxHighlighter
    // =========================================================================

    namespace
    {
        std::unordered_set<std::string> keywordsFor(CodeLanguage lang)
        {
            switch (lang)
            {
            case CodeLanguage::Cpp: return {
                "alignas","auto","bool","break","case","catch","char","class","const",
                "constexpr","continue","decltype","default","delete","do","double","else",
                "enum","explicit","extern","false","float","for","friend","if","inline","int",
                "long","namespace","new","noexcept","nullptr","operator","override","private",
                "protected","public","return","short","signed","sizeof","static","struct",
                "switch","template","this","throw","true","try","typedef","typename","union",
                "unsigned","using","virtual","void","volatile","while","include","define"};
            case CodeLanguage::Python: return {
                "and","as","assert","async","await","break","class","continue","def","del",
                "elif","else","except","False","finally","for","from","global","if","import",
                "in","is","lambda","None","nonlocal","not","or","pass","raise","return","self",
                "True","try","while","with","yield"};
            case CodeLanguage::JavaScript: return {
                "async","await","break","case","catch","class","const","continue","default",
                "delete","do","else","export","extends","false","finally","for","from","function",
                "if","import","in","instanceof","let","new","null","of","return","super","switch",
                "this","throw","true","try","typeof","undefined","var","void","while","yield"};
            case CodeLanguage::Java: return {
                "abstract","boolean","break","byte","case","catch","char","class","const",
                "continue","default","do","double","else","enum","extends","final","finally",
                "float","for","if","implements","import","instanceof","int","interface","long",
                "new","package","private","protected","public","return","short","static","super",
                "switch","this","throw","throws","true","false","try","void","while","null"};
            case CodeLanguage::Plain:
            default: return {};
            }
        }
    }

    BasicSyntaxHighlighter::BasicSyntaxHighlighter(CodeLanguage lang) { setLanguage(lang); }

    void BasicSyntaxHighlighter::setLanguage(CodeLanguage lang)
    {
        _lang         = lang;
        _keywords     = keywordsFor(lang);
        _hashComments = (lang == CodeLanguage::Python);
    }

    std::vector<SyntaxToken> BasicSyntaxHighlighter::tokenize(const std::string& text) const
    {
        std::vector<SyntaxToken> out;
        const std::size_t n = text.size();

        auto isIdentStart = [](unsigned char c){ return std::isalpha(c) || c == '_'; };
        auto isIdent      = [](unsigned char c){ return std::isalnum(c) || c == '_'; };

        std::size_t i = 0;
        while (i < n)
        {
            const char c = text[i];

            // line comment  //…
            if (c == '/' && i + 1 < n && text[i + 1] == '/')
            {
                const std::size_t s = i;
                while (i < n && text[i] != '\n') ++i;
                out.push_back({ s, i, commentColor });
                continue;
            }
            // hash line comment  #…  (Python)
            if (_hashComments && c == '#')
            {
                const std::size_t s = i;
                while (i < n && text[i] != '\n') ++i;
                out.push_back({ s, i, commentColor });
                continue;
            }
            // block comment  /* … */
            if (c == '/' && i + 1 < n && text[i + 1] == '*')
            {
                const std::size_t s = i;
                i += 2;
                while (i + 1 < n && !(text[i] == '*' && text[i + 1] == '/')) ++i;
                i = (i + 1 < n) ? i + 2 : n;
                out.push_back({ s, i, commentColor });
                continue;
            }
            // string  "…"  or  '…'
            if (c == '"' || c == '\'')
            {
                const char q = c;
                const std::size_t s = i;
                ++i;
                while (i < n && text[i] != q)
                {
                    if (text[i] == '\\' && i + 1 < n) ++i;  // skip escaped char
                    ++i;
                }
                if (i < n) ++i;  // closing quote
                out.push_back({ s, i, stringColor });
                continue;
            }
            // number
            if (std::isdigit(static_cast<unsigned char>(c)))
            {
                const std::size_t s = i;
                while (i < n &&
                       (std::isalnum(static_cast<unsigned char>(text[i])) ||
                        text[i] == '.'))
                    ++i;
                out.push_back({ s, i, numberColor });
                continue;
            }
            // identifier / keyword
            if (isIdentStart(static_cast<unsigned char>(c)))
            {
                const std::size_t s = i;
                while (i < n && isIdent(static_cast<unsigned char>(text[i]))) ++i;
                if (_keywords.find(text.substr(s, i - s)) != _keywords.end())
                    out.push_back({ s, i, keywordColor });
                continue;
            }
            ++i;
        }
        return out;
    }

    // =========================================================================
    // CodeEditor
    // =========================================================================

    const sf::Font& CodeEditor::monospaceFont()
    {
        static sf::Font font;
        static bool     tried  = false;
        static bool     loaded = false;
        if (!tried)
        {
            tried = true;
            const char* candidates[] = {
#if defined(__APPLE__)
                "/System/Library/Fonts/Menlo.ttc",
                "/System/Library/Fonts/Monaco.ttf",
                "/System/Library/Fonts/Supplemental/Courier New.ttf",
#elif defined(_WIN32)
                "C:\\Windows\\Fonts\\consola.ttf",
                "C:\\Windows\\Fonts\\cour.ttf",
#else
                "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
                "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf",
                "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
                "/usr/share/fonts/dejavu/DejaVuSansMono.ttf",
#endif
            };
            for (const char* path : candidates)
                if (font.openFromFile(path)) { loaded = true; break; }
        }
        return loaded ? font : FontManager<>::getDefault();
    }

    CodeEditor::CodeEditor(const sf::Font& font)
        : TextArea(font)
    {
        auto basic   = std::make_shared<BasicSyntaxHighlighter>(CodeLanguage::Cpp);
        _basic       = basic.get();
        _highlighter = std::move(basic);
    }

    void CodeEditor::setLanguage(CodeLanguage lang)
    {
        if (_basic) _basic->setLanguage(lang);
        _lastHighlighted.clear();   // force a re-highlight on next rebuild
        rebuild();
    }

    void CodeEditor::setHighlighter(std::shared_ptr<SyntaxHighlighter> highlighter)
    {
        _highlighter = std::move(highlighter);
        _basic       = nullptr;     // no longer the built-in
        _lastHighlighted.clear();
        rebuild();
    }

    void CodeEditor::applyHighlighting()
    {
        _buffer.clearAttributes();
        if (!_highlighter) return;
        for (const SyntaxToken& tok : _highlighter->tokenize(_buffer.getText()))
        {
            TextAttribute attr;
            attr.color = tok.color;
            _buffer.applyAttributeRange(tok.start, tok.end, attr);
        }
    }

    void CodeEditor::onRebuildComplete()
    {
        TextArea::onRebuildComplete();      // grow canvas / scroll bookkeeping

        if (_inHighlight || !_highlighter) return;
        const std::string& txt = _buffer.getText();
        if (txt == _lastHighlighted) return;

        _lastHighlighted = txt;
        _inHighlight     = true;
        applyHighlighting();
        rebuild();                          // re-render with the new colors
        _inHighlight     = false;
    }

} // namespace ml
