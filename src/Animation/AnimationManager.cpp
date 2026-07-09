//
// AnimationManager.cpp — see AnimationManager.h.
//

#include <Malena/Animation/AnimationManager.h>
#include <algorithm>

namespace ml
{
    // Function-local statics: safe initialization order regardless of when the
    // first animation is registered vs. static ctors elsewhere.
    std::vector<AnimationManager::Track>& AnimationManager::tracks()
    {
        static std::vector<Track> t;
        return t;
    }

    std::vector<AnimationManager::Track>& AnimationManager::pending()
    {
        static std::vector<Track> p;
        return p;
    }

    bool& AnimationManager::advancing()
    {
        static bool a = false;
        return a;
    }

    void AnimationManager::add(const void* owner, Step step)
    {
        if (!step) return;
        // Registering during advance() (e.g. from an onComplete callback) must not
        // reallocate the vector we're iterating — stage it and merge afterwards.
        (advancing() ? pending() : tracks()).push_back({ owner, std::move(step), false });
    }

    void AnimationManager::advance(float dt)
    {
        auto& live = tracks();
        if (live.empty()) return;

        advancing() = true;
        for (auto& t : live)
        {
            if (t.dead || !t.step) continue;
            if (t.step(dt))            // returns true when finished
                t.dead = true;
        }
        advancing() = false;

        // Merge any animations registered mid-advance.
        auto& stage = pending();
        if (!stage.empty())
        {
            live.insert(live.end(),
                        std::make_move_iterator(stage.begin()),
                        std::make_move_iterator(stage.end()));
            stage.clear();
        }

        // Compact out finished/cancelled tracks.
        live.erase(std::remove_if(live.begin(), live.end(),
                                  [](const Track& t) { return t.dead || !t.step; }),
                   live.end());
    }

    void AnimationManager::cancel(const void* owner)
    {
        for (auto& t : tracks())
            if (t.owner == owner) t.dead = true;
        // Also drop any staged-but-not-yet-merged tracks for this owner.
        for (auto& t : pending())
            if (t.owner == owner) t.dead = true;
        // If we're not mid-advance, compact immediately so hasActive is accurate.
        if (!advancing())
        {
            auto& live = tracks();
            live.erase(std::remove_if(live.begin(), live.end(),
                                      [](const Track& t) { return t.dead || !t.step; }),
                       live.end());
        }
    }

    bool AnimationManager::hasActive(const void* owner)
    {
        for (const auto& t : tracks())
            if (t.owner == owner && !t.dead && t.step) return true;
        for (const auto& t : pending())
            if (t.owner == owner && !t.dead && t.step) return true;
        return false;
    }

    std::size_t AnimationManager::activeCount()
    {
        std::size_t n = 0;
        for (const auto& t : tracks())   if (!t.dead && t.step) ++n;
        for (const auto& t : pending())  if (!t.dead && t.step) ++n;
        return n;
    }

} // namespace ml
