// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#include <Malena/Screen/ScreenLayout.h>
#include <algorithm>

namespace ml {

ScreenLayout::ScreenLayout()
    : _pane0("ScreenLayout.0", ""),
      _pane1("ScreenLayout.1", "")
{
    this->setFillColor(sf::Color::Black);
    this->setSize({ 640.f, 360.f });

    // The panes are drawn by hand in draw(), but they must still be LINKED into
    // the component tree. Malena's rule: anything that visually attaches a Core*
    // has to call linkChild, or that component is invisible to the enable
    // cascade and to tree walks like topmostMatching. Unlinked, these panes
    // stayed ENABLED while their scene was inactive and — sitting at (0,0) with
    // their default size, never having been laid out — kept matching clicks over
    // whatever was actually on screen, stealing focus from the login fields.
    // linkChild (not addComponent) because draw() paints them itself; adding
    // them to the draw list too would paint them twice.
    Core::linkChild(this, &_pane0);
    Core::linkChild(this, &_pane1);
}

void ScreenLayout::setMode(Mode mode) { _mode = mode; }
ScreenLayout::Mode ScreenLayout::mode() const { return _mode; }

void ScreenLayout::setRatio(float ratio01)
{
    _ratio = std::clamp(ratio01, 0.05f, 0.95f);
}
float ScreenLayout::ratio() const { return _ratio; }

void ScreenLayout::setSource(int pane, const std::string& rtspUrl)
{
    ScreenReceiverBase& r = (pane == 1) ? _pane1 : _pane0;
    r.setUrl(rtspUrl);
    if (_running && !rtspUrl.empty()) r.start();
}

void ScreenLayout::setFrozen(int pane, bool frozen)
{
    ((pane == 1) ? _pane1 : _pane0).setFrozen(frozen);
}

void ScreenLayout::setScaleMode(ScreenReceiverBase::ScaleMode mode)
{
    _pane0.setScaleMode(mode);
    _pane1.setScaleMode(mode);
}

void ScreenLayout::start()
{
    _running = true;
    if (!_pane0.url().empty()) _pane0.start();
    if (!_pane1.url().empty()) _pane1.start();
}

void ScreenLayout::stop()
{
    _running = false;
    _pane0.stop();
    _pane1.stop();
}

void ScreenLayout::layoutPanes() const
{
    const sf::Vector2f pos = this->getPosition();
    const sf::Vector2f sz  = this->getSize();

    switch (_mode)
    {
    case Mode::Single:
        _pane0.setPosition(pos);
        _pane0.setSize(sz);
        break;

    case Mode::SideBySide:
    {
        const float w0 = sz.x * _ratio;
        _pane0.setPosition(pos);
        _pane0.setSize({ w0, sz.y });
        _pane1.setPosition({ pos.x + w0, pos.y });
        _pane1.setSize({ sz.x - w0, sz.y });
        break;
    }

    case Mode::AboveBelow:
    {
        const float h0 = sz.y * _ratio;
        _pane0.setPosition(pos);
        _pane0.setSize({ sz.x, h0 });
        _pane1.setPosition({ pos.x, pos.y + h0 });
        _pane1.setSize({ sz.x, sz.y - h0 });
        break;
    }
    }
}

void ScreenLayout::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    layoutPanes();
    _pane0.draw(target, states);
    if (_mode != Mode::Single)
        _pane1.draw(target, states);
}

void ScreenLayout::pushFrame(int pane, const std::uint8_t* data, std::size_t size)
{
    (pane == 1 ? _pane1 : _pane0).pushFrame(data, size);
}

} // namespace ml
