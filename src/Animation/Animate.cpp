//
// Animate.cpp — see Animate.h.
//

#include <Malena/Animation/Animate.h>
#include <Malena/Traits/Spatial/Positionable.h>
#include <memory>

namespace ml
{
    Animate& Animate::move(sf::Vector2f to, float seconds, Easing::Fn ease,
                           std::function<void()> onComplete)
    {
        if (!_owner) return *this;

        // A new positional move supersedes whatever this object was doing, so two
        // position tweens never both write setPosition on the same frame.
        cancel();

        const sf::Vector2f from = _owner->getPosition();
        Positionable* owner = _owner;

        auto tw = std::make_shared<Tweener<sf::Vector2f>>(from, to, seconds, ease);
        tw->onUpdate([owner](const sf::Vector2f& p) { owner->setPosition(p); });
        if (onComplete) tw->onComplete(std::move(onComplete));

        AnimationManager::add(_owner, [tw](float dt) { tw->update(dt); return tw->done(); });
        return *this;
    }

    Animate& Animate::moveBy(sf::Vector2f delta, float seconds, Easing::Fn ease,
                             std::function<void()> onComplete)
    {
        if (!_owner) return *this;
        return move(_owner->getPosition() + delta, seconds, ease, std::move(onComplete));
    }

    void Animate::cancel()
    {
        AnimationManager::cancel(_owner);
    }

    bool Animate::active() const
    {
        return AnimationManager::hasActive(_owner);
    }

} // namespace ml
