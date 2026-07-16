// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

//
// DynamicPanel — a retained container for data-driven UI you rebuild declaratively.
//
// You describe the contents in an onBuild(builder) callback using text()/button()/
// rect()/input(); the panel POOLS the underlying widgets (reuses them across
// rebuilds, hides leftovers) so nothing is ever destroyed — and it defers the
// actual rebuild to a frame boundary, so calling invalidate() from inside a
// button's own click handler is safe (no rebuild-during-dispatch UAF). Text
// inputs are keyed by a stable id so the same physical field survives a rebuild
// (focus/typing preserved) and are auto-parked when a rebuild stops using them.
//
// This replaces the hand-rolled "pool + _dirty/onUpdate + park at -100000"
// boilerplate that data-driven hooks otherwise re-implement.
//
#ifndef MALENA_DYNAMICPANEL_H
#define MALENA_DYNAMICPANEL_H

#include <Malena/Graphics/Layouts/Panel.h>
#include <Malena/Graphics/Controls/RectangleButton.h>
#include <Malena/Graphics/Text/Text.h>
#include <Malena/Graphics/Text/TextInput.h>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace ml {

class MALENA_API DynamicPanel : public Panel
{
public:
    DynamicPanel();

    /// The builder handed to your onBuild callback. Each call reuses a pooled
    /// widget positioned RELATIVE to the panel's own origin.
    class MALENA_API Builder
    {
        DynamicPanel& _p;
        friend class DynamicPanel;
        explicit Builder(DynamicPanel& p) : _p(p) {}
    public:
        /// A label. Pass center=true to horizontally centre it on x.
        Text& text(const std::string& s, float x, float y, unsigned size,
                   const sf::Color& color, bool center = false);

        /// A clickable button. `style` is passed to the styler set via
        /// setButtonStyler (0 if none set); the callback replaces any prior one.
        RectangleButton& button(const std::string& label, float x, float y, float w, float h,
                                int style, std::function<void()> onClick);

        /// A non-interactive coloured rectangle (decor / background / divider).
        RectangleButton& rect(float x, float y, float w, float h, const sf::Color& fill);

        /// A text field, keyed by a STABLE id so the same physical input survives
        /// rebuilds (focus/typing preserved). `value` seeds the field only when it
        /// isn't focused (never clobbers typing); `onChange` replaces any prior one.
        TextInput& input(const std::string& id, const std::string& placeholder,
                         float x, float y, float w, float h, const std::string& value,
                         std::function<void(const std::string&)> onChange);
    };

    /// Declare the panel's content. Runs on the next rebuild.
    void onBuild(std::function<void(Builder&)> fn);

    /// Set how `Builder::button`'s `style` int maps to a look (e.g. wrap your
    /// app's styleButton). Optional — without it buttons keep their default look.
    void setButtonStyler(std::function<void(RectangleButton&, int /*style*/, float /*w*/, float /*h*/)> styler);

    /// Request a rebuild. Safe to call from inside a click/onChange handler — the
    /// rebuild happens at the next frame boundary, never mid-dispatch.
    void invalidate() { _dirty = true; }

    /// True while a rebuild is pending (set by invalidate/onBuild, cleared when the
    /// deferred rebuild runs). Exposed mainly for tests.
    bool isRebuildPending() const { return _dirty; }

protected:
    /// Run the pooled rebuild NOW. The framework calls this at a frame boundary via
    /// invalidate(); tests may call it directly to drive a synchronous rebuild.
    void doRebuild();

private:
    RectangleButton& acquireButton();
    Text&            acquireText();

    std::function<void(Builder&)> _build;
    std::function<void(RectangleButton&, int, float, float)> _styler;

    std::vector<std::unique_ptr<RectangleButton>> _buttons;
    std::vector<std::unique_ptr<Text>>            _texts;
    std::map<std::string, std::unique_ptr<TextInput>> _inputs;

    std::size_t          _btnUsed = 0, _txtUsed = 0;
    std::set<std::string> _usedInputs;
    bool                 _dirty = false;
};

} // namespace ml

#endif // MALENA_DYNAMICPANEL_H
