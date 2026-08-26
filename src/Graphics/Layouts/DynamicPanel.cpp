// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#include <Malena/Graphics/Layouts/DynamicPanel.h>
#include <Malena/Utilities/Utf8.h>

namespace ml {

DynamicPanel::DynamicPanel()
{
    // Rebuild at a frame boundary, never during click dispatch — so a button
    // whose handler calls invalidate() (rebuilding this very panel) is safe.
    onUpdate([this]() { if (_dirty) doRebuild(); });
}

void DynamicPanel::onBuild(std::function<void(Builder&)> fn)
{
    _build = std::move(fn);
    _dirty = true;
}

void DynamicPanel::setButtonStyler(std::function<void(RectangleButton&, int, float, float)> styler)
{
    _styler = std::move(styler);
}

RectangleButton& DynamicPanel::acquireButton()
{
    if (_btnUsed >= _buttons.size())
    {
        auto b = std::make_unique<RectangleButton>();
        b->setVisibilityIndependent(true);   // the panel's own setVisible won't force-show unused ones
        addComponent(*b);
        _buttons.push_back(std::move(b));
    }
    return *_buttons[_btnUsed++];
}

Text& DynamicPanel::acquireText()
{
    if (_txtUsed >= _texts.size())
    {
        auto t = std::make_unique<Text>();
        t->setVisibilityIndependent(true);
        addComponent(*t);
        _texts.push_back(std::move(t));
    }
    return *_texts[_txtUsed++];
}

void DynamicPanel::doRebuild()
{
    _dirty = false;
    _btnUsed = 0; _txtUsed = 0; _usedInputs.clear();

    if (_build) { Builder b(*this); _build(b); }

    // Hide the pooled widgets this pass didn't use, and park unused inputs.
    for (std::size_t i = _btnUsed; i < _buttons.size(); ++i) { _buttons[i]->setVisible(false); _buttons[i]->setEnabled(false); }
    for (std::size_t i = _txtUsed; i < _texts.size(); ++i)     _texts[i]->setVisible(false);
    for (auto& entry : _inputs)
        if (_usedInputs.find(entry.first) == _usedInputs.end())
        {
            entry.second->setVisible(false);
            entry.second->setEnabled(false);
            entry.second->setPosition({ -100000.f, -100000.f });
        }
}

// ── Builder ───────────────────────────────────────────────────────────────────
Text& DynamicPanel::Builder::text(const std::string& s, float x, float y, unsigned size,
                                  const sf::Color& color, bool center)
{
    Text& t = _p.acquireText();
    t.setString(ml::utf8(s));
    t.setCharacterSize(size);
    t.setFillColor(color);
    float px = x;
    if (center) { auto b = t.getLocalBounds(); px -= (b.position.x + b.size.x) * 0.5f; }
    t.setPosition(_p.getPosition() + sf::Vector2f{ px, y });
    t.setVisible(true);
    return t;
}

RectangleButton& DynamicPanel::Builder::button(const std::string& label, float x, float y, float w, float h,
                                               int style, std::function<void()> onClick)
{
    RectangleButton& b = _p.acquireButton();
    b.setString(ml::utf8(label));
    if (_p._styler) _p._styler(b, style, w, h);
    else            b.setSize({ w, h });
    b.setPosition(_p.getPosition() + sf::Vector2f{ x, y });
    b.onClick(onClick ? std::move(onClick) : std::function<void()>([]{}));   // replaces any prior handler
    b.setVisible(true);
    b.setEnabled(true);
    return b;
}

RectangleButton& DynamicPanel::Builder::rect(float x, float y, float w, float h, const sf::Color& fill)
{
    RectangleButton& r = _p.acquireButton();
    r.setString("");
    r.setSize({ w, h });
    r.setPosition(_p.getPosition() + sf::Vector2f{ x, y });
    r.onClick([]{});
    r.setFillColor(fill);
    r.lockTheme();
    r.setVisible(true);
    r.setEnabled(false);   // decor — never intercepts clicks
    return r;
}

TextInput& DynamicPanel::Builder::input(const std::string& id, const std::string& placeholder,
                                        float x, float y, float w, float h, const std::string& value,
                                        std::function<void(const std::string&)> onChange)
{
    auto it = _p._inputs.find(id);
    if (it == _p._inputs.end())
    {
        auto in = std::make_unique<TextInput>();
        in->setVisibilityIndependent(true);
        _p.addComponent(*in);
        it = _p._inputs.emplace(id, std::move(in)).first;
    }
    TextInput& in = *it->second;
    _p._usedInputs.insert(id);

    in.setPlaceholder(placeholder);
    in.setSize({ w, h });
    in.setPosition(_p.getPosition() + sf::Vector2f{ x, y });
    in.onChange(onChange ? std::move(onChange) : std::function<void(const std::string&)>([](const std::string&){}));
    if (!in.isFocused() && in.getValue() != value) in.setValue(value);   // seed without clobbering typing
    in.setVisible(true);
    in.setEnabled(true);
    return in;
}

} // namespace ml
