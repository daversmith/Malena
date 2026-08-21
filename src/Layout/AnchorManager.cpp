//
// AnchorManager.cpp — see AnchorManager.h.
//

#include <Malena/Layout/AnchorManager.h>
#include <Malena/Traits/Spatial/Positionable.h>
#include <Malena/Utilities/Align.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <SFML/Graphics/Rect.hpp>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>

namespace ml
{
    struct AnchorManager::Entry
    {
        std::vector<Anchor> anchors;
        sf::Vector2f        baseline{};
        bool                hasBaseline = false;
    };

    std::unordered_map<Positionable*, AnchorManager::Entry>& AnchorManager::registry()
    {
        static std::unordered_map<Positionable*, Entry> m;
        return m;
    }

    namespace
    {
        bool moved(const sf::Vector2f& a, const sf::Vector2f& b, float eps = 0.5f)
        {
            return std::fabs(a.x - b.x) > eps || std::fabs(a.y - b.y) > eps;
        }

        sf::FloatRect windowRect()
        {
            const auto sz = static_cast<sf::Vector2f>(WindowManager::getWindow().getSize());
            return sf::FloatRect({ 0.f, 0.f }, sz);
        }
    }

    void AnchorManager::set(Positionable* owner, AnchorOp op, const Positionable* ref, float spacing)
    {
        if (!owner) return;
        auto& e = registry()[owner];
        // An op replaces any existing anchor with the same op, so a fill() that
        // re-runs the same verbs each resize never accumulates duplicates.
        e.anchors.erase(std::remove_if(e.anchors.begin(), e.anchors.end(),
                                       [op](const Anchor& a) { return a.op == op; }),
                        e.anchors.end());
        e.anchors.push_back({ op, ref, spacing });
    }

    void AnchorManager::recordBaseline(Positionable* owner)
    {
        auto it = registry().find(owner);
        if (it == registry().end()) return;
        it->second.baseline    = owner->getPosition();
        it->second.hasBaseline = true;
    }

    void AnchorManager::clear(Positionable* owner)
    {
        registry().erase(owner);
    }

    bool AnchorManager::hasAnchors(const Positionable* owner)
    {
        auto it = registry().find(const_cast<Positionable*>(owner));
        return it != registry().end() && !it->second.anchors.empty();
    }

    void AnchorManager::release(const Positionable* owner)
    {
        auto& m = registry();
        m.erase(const_cast<Positionable*>(owner));          // its own anchors
        for (auto it = m.begin(); it != m.end(); )          // scrub references to it
        {
            auto& v = it->second.anchors;
            v.erase(std::remove_if(v.begin(), v.end(),
                                   [owner](const Anchor& a) { return a.ref == owner; }),
                    v.end());
            if (v.empty()) it = m.erase(it);
            else           ++it;
        }
    }

    void AnchorManager::solveOwner(Positionable* owner, Entry& e)
    {
        for (const Anchor& a : e.anchors)
        {
            const sf::FloatRect r = a.ref ? a.ref->getGlobalBounds() : windowRect();
            switch (a.op)
            {
                case AnchorOp::LeftOf:   Align::setLeftOf(r, *owner, a.spacing);        break;
                case AnchorOp::RightOf:  Align::setRightOf(r, *owner, a.spacing);       break;
                case AnchorOp::Above:    Align::setAbove(r, *owner, a.spacing);         break;
                case AnchorOp::Below:    Align::setBelow(r, *owner, a.spacing);         break;
                case AnchorOp::CenterX:
                    Align::centerHorizontally(r, *owner);
                    if (a.spacing != 0.f)
                        owner->setPosition({ owner->getPosition().x + a.spacing, owner->getPosition().y });
                    break;
                case AnchorOp::CenterY:
                    Align::centerVertically(r, *owner);
                    if (a.spacing != 0.f)
                        owner->setPosition({ owner->getPosition().x, owner->getPosition().y + a.spacing });
                    break;
                case AnchorOp::CenterOn: Align::centerOn(r, *owner);                    break;

                // Window inside-edge ops: r is the window rect; keep the object's
                // position→visual-bounds delta so outlined shapes land correctly.
                case AnchorOp::WindowLeft:
                case AnchorOp::WindowRight:
                case AnchorOp::WindowTop:
                case AnchorOp::WindowBottom:
                {
                    const sf::FloatRect b = owner->getGlobalBounds();
                    sf::Vector2f p = owner->getPosition();
                    const sf::Vector2f off = { p.x - b.position.x, p.y - b.position.y };
                    if (a.op == AnchorOp::WindowLeft)   p.x = r.position.x + a.spacing + off.x;
                    if (a.op == AnchorOp::WindowRight)  p.x = r.position.x + r.size.x - b.size.x - a.spacing + off.x;
                    if (a.op == AnchorOp::WindowTop)    p.y = r.position.y + a.spacing + off.y;
                    if (a.op == AnchorOp::WindowBottom) p.y = r.position.y + r.size.y - b.size.y - a.spacing + off.y;
                    owner->setPosition(p);
                    break;
                }
            }
        }
    }

    void AnchorManager::solveAll()
    {
        auto& m = registry();
        if (m.empty()) return;

        // 1) An object moved since its last solve was moved by something else (a
        //    direct setPosition, or an animation) — manual control wins, so detach.
        std::vector<Positionable*> detach;
        for (auto& kv : m)
            if (!kv.second.anchors.empty() && kv.second.hasBaseline
                && moved(kv.first->getPosition(), kv.second.baseline))
                detach.push_back(kv.first);
        for (auto* o : detach) m.erase(o);

        // 2) Iterate to a fixpoint so an object anchored to a sibling settles after
        //    that sibling, whatever the registration order. Capped to break cycles.
        constexpr int kMaxPasses = 8;
        for (int pass = 0; pass < kMaxPasses; ++pass)
        {
            bool changed = false;
            for (auto& kv : m)
            {
                if (kv.second.anchors.empty()) continue;
                const sf::Vector2f before = kv.first->getPosition();
                solveOwner(kv.first, kv.second);
                if (moved(kv.first->getPosition(), before, 0.01f)) changed = true;
            }
            if (!changed) break;
        }

        // 3) Refresh baselines so the next solve can tell "solver moved it" from
        //    "someone else moved it".
        for (auto& kv : m)
        {
            if (kv.second.anchors.empty()) continue;
            kv.second.baseline    = kv.first->getPosition();
            kv.second.hasBaseline = true;
        }
    }

    std::size_t AnchorManager::anchoredCount()
    {
        std::size_t n = 0;
        for (const auto& kv : registry())
            if (!kv.second.anchors.empty()) ++n;
        return n;
    }

} // namespace ml
