// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_CODEEDITOR_H
#define MALENA_CODEEDITOR_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Graphics/Text/TextArea.h>
#include <SFML/Graphics/Color.hpp>
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace ml
{
    // ── Languages ───────────────────────────────────────────────────────────────
    enum class CodeLanguage { Plain, Cpp, Python, JavaScript, Java };

    // ── SyntaxToken ───────────────────────────────────────────────────────────────
    /** A colored span of source text (half-open byte range [start, end)). */
    struct SyntaxToken
    {
        std::size_t start = 0;
        std::size_t end   = 0;
        sf::Color   color;
    };

    // ── SyntaxHighlighter ─────────────────────────────────────────────────────────
    /**
     * @brief Pure tokenizer: maps source text to colored spans. No linting,
     *        no diagnostics — color only. Subclass for custom languages.
     */
    class MALENA_API SyntaxHighlighter
    {
    public:
        virtual ~SyntaxHighlighter() = default;
        /** Return colored spans for @p text (only the spans that need coloring;
         *  everything else is left at the editor's default text color). */
        [[nodiscard]] virtual std::vector<SyntaxToken> tokenize(const std::string& text) const = 0;
    };

    // ── BasicSyntaxHighlighter ──────────────────────────────────────────────────
    /**
     * @brief Built-in generic highlighter for C-like and scripting languages.
     *
     * Recognizes line comments (// and, for Python, #), block comments,
     * single/double-quoted strings, numbers, and a per-language keyword set.
     * Colors are public and tweakable (defaults approximate a dark theme).
     */
    class MALENA_API BasicSyntaxHighlighter : public SyntaxHighlighter
    {
    public:
        explicit BasicSyntaxHighlighter(CodeLanguage lang = CodeLanguage::Cpp);

        void setLanguage(CodeLanguage lang);
        [[nodiscard]] CodeLanguage language() const { return _lang; }

        [[nodiscard]] std::vector<SyntaxToken> tokenize(const std::string& text) const override;

        // Theme colors (override freely).
        sf::Color keywordColor { 197, 134, 192 };   // purple
        sf::Color stringColor  { 206, 145, 120 };   // orange
        sf::Color commentColor { 106, 153,  85 };   // green
        sf::Color numberColor  { 181, 206, 168 };   // light green

    private:
        CodeLanguage                     _lang;
        std::unordered_set<std::string>  _keywords;
        bool                             _hashComments = false; // '#' line comments
    };

    // ── CodeEditor ──────────────────────────────────────────────────────────────
    /**
     * @brief A multi-line text editor with syntax color-coding.
     * @ingroup GraphicsText
     *
     * A @c TextArea that re-tokenizes its content on every change and applies
     * per-token colors via the rich-text buffer. Color highlighting only — it is
     * deliberately not a linter. Defaults to a monospace system font when one is
     * available (Menlo / DejaVuSansMono / Consolas), else the framework default.
     *
     * @code
     * ml::CodeEditor editor;
     * editor.setLanguage(ml::CodeLanguage::Python);
     * editor.setValue("def hello():\n    return 42  # answer");
     * @endcode
     *
     * @see TextArea, SyntaxHighlighter, BasicSyntaxHighlighter
     */
    class MALENA_API CodeEditor : public TextArea
    {
    public:
        /** @return A cached monospace system font, or the framework default. */
        static const sf::Font& monospaceFont();

        explicit CodeEditor(const sf::Font& font = monospaceFont());

        /** Switch the built-in highlighter's language (ignored if a custom
         *  highlighter was set via @c setHighlighter). */
        void setLanguage(CodeLanguage lang);

        /** Replace the highlighter. Pass nullptr to disable highlighting. */
        void setHighlighter(std::shared_ptr<SyntaxHighlighter> highlighter);

    protected:
        void onRebuildComplete() override;

    private:
        void applyHighlighting();

        std::shared_ptr<SyntaxHighlighter> _highlighter;
        BasicSyntaxHighlighter*            _basic = nullptr; // non-owning alias when built-in
        std::string                        _lastHighlighted;
        bool                               _inHighlight = false;
    };

} // namespace ml

#endif // MALENA_CODEEDITOR_H
