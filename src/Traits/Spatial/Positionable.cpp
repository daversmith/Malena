//
// Created by Dave Smith on 11/13/22.
//

#include <Malena/Traits/Spatial/Positionable.h>
#include <Malena/Animation/Animate.h>
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
    }

    Positionable::~Positionable()
    {
        auto& m = animators();
        auto it = m.find(this);
        if (it != m.end())
            m.erase(it);        // ~Animate cancels any in-flight animations
    }

    Animate& Positionable::animate()
    {
        auto& slot = animators()[this];
        if (!slot)
            slot = std::make_unique<Animate>(this);
        return *slot;
    }

    void Positionable::centerText(sf::Text& obj)
    {
        Align::centerText(*this, obj);
    }

} // namespace ml
