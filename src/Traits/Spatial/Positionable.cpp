//
// Created by Dave Smith on 11/13/22.
//

#include <Malena/Traits/Spatial/Positionable.h>
#include <Malena/Animation/Animate.h>
#include <Malena/Layout/Anchor.h>
#include <Malena/Layout/AnchorManager.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <memory>
#include <unordered_map>

namespace ml
{
    namespace
    {
        // Animate controllers live outside Positionable so the trait keeps no data
        // member (a unique_ptr member would delete the copy-assignment the framework
        // uses for value-type shapes). Keyed by owner pointer; created lazily,
        // released in ~Positionable. Owners are identity objects, so the pointer is
        // a stable key for the object's lifetime.
        std::unordered_map<const Positionable*, std::unique_ptr<Animate>>& animators()
        {
            static std::unordered_map<const Positionable*, std::unique_ptr<Animate>> m;
            return m;
        }

        sf::FloatRect windowBounds()
        {
            const auto sz = static_cast<sf::Vector2f>(WindowManager::getWindow().getSize());
            return sf::FloatRect({ 0.f, 0.f }, sz);
        }
    }

    Positionable::~Positionable()
    {
        auto& m = animators();
        auto it = m.find(this);
        if (it != m.end())
            m.erase(it);            // ~Animate cancels any in-flight animations

        // Drop this object's anchors AND any anchor that referenced it, so the
        // solver never dereferences a destroyed object.
        AnchorManager::release(this);
    }

    Animate& Positionable::animate()
    {
        auto& slot = animators()[this];
        if (!slot)
            slot = std::make_unique<Animate>(this);
        return *slot;
    }

    void Positionable::unanchored()
    {
        AnchorManager::clear(this);
    }

    void Positionable::centerInWindow()
    {
        Align::centerOn(windowBounds(), *this);
        AnchorManager::set(this, AnchorOp::CenterOn, nullptr, 0.f);
        AnchorManager::recordBaseline(this);
    }

    void Positionable::centerXInWindow(float offset)
    {
        Align::centerHorizontally(windowBounds(), *this);
        if (offset != 0.f) setPosition({ getPosition().x + offset, getPosition().y });
        AnchorManager::set(this, AnchorOp::CenterX, nullptr, offset);
        AnchorManager::recordBaseline(this);
    }

    void Positionable::centerYInWindow(float offset)
    {
        Align::centerVertically(windowBounds(), *this);
        if (offset != 0.f) setPosition({ getPosition().x, getPosition().y + offset });
        AnchorManager::set(this, AnchorOp::CenterY, nullptr, offset);
        AnchorManager::recordBaseline(this);
    }

    namespace
    {
        // Immediate placement of a window inside-edge op — mirrors the solver so
        // the object lands correctly before the first resize.
        void applyWindowEdge(Positionable& self, AnchorOp op, float margin)
        {
            const sf::FloatRect w = windowBounds();
            const sf::FloatRect b = self.getGlobalBounds();
            sf::Vector2f p = self.getPosition();
            // Position is p; the visual bounds may be offset from p — preserve that delta.
            const sf::Vector2f off = { p.x - b.position.x, p.y - b.position.y };
            switch (op)
            {
                case AnchorOp::WindowLeft:   p.x = w.position.x + margin + off.x; break;
                case AnchorOp::WindowRight:  p.x = w.position.x + w.size.x - b.size.x - margin + off.x; break;
                case AnchorOp::WindowTop:    p.y = w.position.y + margin + off.y; break;
                case AnchorOp::WindowBottom: p.y = w.position.y + w.size.y - b.size.y - margin + off.y; break;
                default: break;
            }
            self.setPosition(p);
        }

        void setWindowEdge(Positionable* self, AnchorOp op, float margin)
        {
            applyWindowEdge(*self, op, margin);
            AnchorManager::set(self, op, nullptr, margin);
            AnchorManager::recordBaseline(self);
        }
    }

    void Positionable::anchorLeftInWindow(float margin)   { setWindowEdge(this, AnchorOp::WindowLeft,   margin); }
    void Positionable::anchorRightInWindow(float margin)  { setWindowEdge(this, AnchorOp::WindowRight,  margin); }
    void Positionable::anchorTopInWindow(float margin)    { setWindowEdge(this, AnchorOp::WindowTop,    margin); }
    void Positionable::anchorBottomInWindow(float margin) { setWindowEdge(this, AnchorOp::WindowBottom, margin); }

    void Positionable::centerText(sf::Text& obj)
    {
        Align::centerText(*this, obj);
    }

} // namespace ml
