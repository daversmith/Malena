// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_RICHTEXTEDITOR_H
#define MALENA_RICHTEXTEDITOR_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Graphics/Layouts/Panel.h>
#include <Malena/Graphics/Text/TextArea.h>
#include <Malena/Graphics/Text/TextInput.h>   // type-in font size box
#include <Malena/Graphics/Controls/Toolbar.h>
#include <Malena/Graphics/Controls/ColorChooser.h>
#include <Malena/Graphics/Controls/ColorButton.h>
#include <Malena/Graphics/Controls/ContextMenu.h>   // hamburger pop-up when narrow
#include <Malena/Graphics/Controls/Select.h>
#include <Malena/Graphics/Text/CodeEditor.h>   // monospaceFont() for the Code heading
#include <Malena/Resources/IconFont.h>         // iconFont() + Icon glyph codepoints
#include <cstddef>
#include <functional>
#include <string>

namespace ml
{
    /**
     * @brief A WYSIWYG rich-text editor: a formatting toolbar over a TextArea.
     * @ingroup GraphicsText
     *
     * TinyMCE-style. The toolbar (an @c ml::Toolbar, so its clicks dispatch
     * reliably) provides bold / italic / underline toggles and color choices
     * that apply to the current selection (or the next typed text). Content
     * round-trips losslessly as JSON spans via @c getRichText / @c setRichText.
     *
     * @see TextArea, Toolbar, TextInput::getRichText, CodeEditor
     */
    class MALENA_API RichTextEditor : public Panel
    {
    public:
        RichTextEditor();

        // ── Content ───────────────────────────────────────────────────────────
        void setValue(const std::string& text);
        [[nodiscard]] std::string getValue() const;
        void setRichText(const std::string& json);
        [[nodiscard]] std::string getRichText() const;

        void onChange(std::function<void(const std::string&)> cb);

        /**
         * @brief When enabled, the formatting toolbar is hidden until the editor is
         * "active" (the user clicks inside it) and hides again when they click out —
         * so it reads as a plain text area until focused. The text fills the full
         * height while collapsed.
         */
        void setAutoHideToolbar(bool enabled);

        // ── Layout ────────────────────────────────────────────────────────────
        void setPosition(const sf::Vector2f& pos) override;
        void setSize(const sf::Vector2f& size);
        // Hiding the editor closes its color/style pop-ups and collapses it, so a
        // pop-up left open can't linger (e.g. when a parent swaps editors).
        void setVisible(bool visible) override;

        /** Close any open color/style pop-up immediately. */
        void closePopups();

    private:
        void layout();
        void updateActiveState();   // poll clicks to toggle _active (auto-hide)
        bool _autoHideToolbar = false;
        bool _active          = false;
        bool _prevMouseDown   = false;
        void rehighlightCode();                                    // re-color all code paragraphs
        void highlightCodeRange(std::size_t a, std::size_t b,
                                const std::string& text);          // one paragraph
        bool _inCodeHl = false;                                    // re-entrancy guard

        Toolbar      _bar{ iconFont() };   // B/I/U rendered as icon glyphs
        ColorButton  _colorBtn;            // color swatch (shows last-chosen color)
        TextArea     _area;
        ColorChooser _color;
        TextInput    _sizeInput;           // type-in font size (mirrors caret)
        Select       _headingSelect;
        ContextMenu  _alignMenu;           // align + list when collapsed (hamburger)

        static constexpr unsigned int kMinFontSize = 6;
        static constexpr unsigned int kMaxFontSize = 200;   // sane cap (see notes)
        static constexpr float        kCompactBelow = 520.f; // collapse align/list below this width

        std::size_t _iBold = 0, _iItalic = 0, _iUnderline = 0;
        std::size_t _iAlignL = 0, _iAlignC = 0, _iAlignR = 0;
        std::size_t _iListBullet = 0, _iListNum = 0;
        std::size_t _iMenu = 0;
        bool _b = false, _i = false, _u = false;
        bool _compact = false;   // align/list collapsed into the hamburger
        bool _syncing = false;   // mirroring caret → suppress control callbacks
        int  _curAlign = 0;   // 0 left / 1 center / 2 right
        int  _curList  = 0;   // 0 none / 1 bullet / 2 numbered
        std::string _curStyle = "Paragraph";   // last style shown in the dropdown

        // Apply a paragraph-level change: expand to the cursor's paragraph, run
        // the styling callback, then restore the caret where the user was typing.
        void applyToParagraph(const std::function<void()>& fn);
        void setParagraphAlign(int align);
        void toggleList(int listType);

        void buildBar(bool compact);     // (re)populate the toolbar for the mode
        void openAlignListMenu();        // show the hamburger pop-up

        // Mirror every toolbar control to the style at the caret.
        void syncControlsToCursor();

        // Selection captured when a pop-up/dropdown opens, re-applied before each1
        // change (overlays drop the live selection otherwise).
        std::size_t _colorSelStart = 0, _colorSelEnd = 0;
        std::size_t _hdSelStart    = 0, _hdSelEnd    = 0;

        std::function<void(const std::string&)> _onChange;
    };

} // namespace ml

#endif // MALENA_RICHTEXTEDITOR_H
