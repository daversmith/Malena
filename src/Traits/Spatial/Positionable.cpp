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

    void Positionable::centerXInWindow()
    {
        Align::centerHorizontally(windowBounds(), *this);
        AnchorManager::set(this, AnchorOp::CenterX, nullptr, 0.f);
        AnchorManager::recordBaseline(this);
    }

    void Positionable::centerYInWindow()
    {
        Align::centerVertically(windowBounds(), *this);
        AnchorManager::set(this, AnchorOp::CenterY, nullptr, 0.f);
        AnchorManager::recordBaseline(this);
    }

    void Positionable::centerText(sf::Text& obj)
    {
        Align::centerText(*this, obj);
    }

} // namespace ml
