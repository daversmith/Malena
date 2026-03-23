#pragma once
// ============================================================
//  src/Traits/Fadeable.tpp
// ============================================================

#include <Malena/Traits/Fadeable.h>
#include <Malena/Engine/Events/EventsManager.h>
#include <algorithm>
#include <cmath>

namespace ml
{

// ── Constructor / Destructor ──────────────────────────────────────────────────

Fadeable::Fadeable()
{
    // Subscribe to the framework's per-frame update event.
    // `this` is used as the subscriber identity so unsubscription
    // on destruction is automatic (EventsManager cleans up by pointer).
    EventsManager::subscribe("update", this,
        [this](const std::optional<sf::Event>&)
        {
            tick();
        });
}

Fadeable::~Fadeable()
{
    EventsManager::unsubscribeAll(this);
}

// ── Public API ────────────────────────────────────────────────────────────────

void Fadeable::fadeTo(uint8_t               targetAlpha,
                      float                 duration,
                      Tween                 tween,
                      std::function<void()> onComplete)
{
    _startAlpha  = _alpha;
    _targetAlpha = static_cast<float>(targetAlpha);
    _duration    = std::max(duration, 0.001f);  // guard against zero
    _elapsed     = 0.f;
    _tween       = tween;
    _onComplete  = onComplete;
    _active      = true;
    _holding     = false;
}

void Fadeable::fadeIn(float duration, Tween tween, std::function<void()> onComplete)
{
    fadeTo(255, duration, tween, onComplete);
}

void Fadeable::fadeOut(float duration, Tween tween, std::function<void()> onComplete)
{
    fadeTo(0, duration, tween, onComplete);
}

void Fadeable::holdFor(float duration, std::function<void()> onComplete)
{
    _duration   = std::max(duration, 0.001f);
    _elapsed    = 0.f;
    _onComplete = onComplete;
    _active     = true;
    _holding    = true;
    // alpha is unchanged — we are just waiting
}

void Fadeable::setAlpha(uint8_t alpha)
{
    _alpha      = static_cast<float>(alpha);
    _active     = false;
    _holding    = false;
    _onComplete = nullptr;
}

uint8_t Fadeable::getAlpha() const
{
    return static_cast<uint8_t>(std::clamp(_alpha, 0.f, 255.f));
}

bool Fadeable::isFading() const
{
    return _active;
}

// ── Internal tick ─────────────────────────────────────────────────────────────

void Fadeable::tick()
{
    if (!_active) return;

    constexpr float DT = 1.f / 60.f;   // fixed timestep matching framework default
    _elapsed += DT;

    float t = std::clamp(_elapsed / _duration, 0.f, 1.f);

    if (!_holding)
    {
        // Interpolate alpha using the chosen easing curve
        float easedT = applyTween(t, _tween);
        _alpha = _startAlpha + (_targetAlpha - _startAlpha) * easedT;
    }

    // Animation complete?
    if (t >= 1.f)
    {
        if (!_holding) _alpha = _targetAlpha;  // snap to exact target
        _active  = false;
        _holding = false;

        // Fire completion callback then clear it — fires exactly once
        if (_onComplete)
        {
            auto cb = std::move(_onComplete);
            _onComplete = nullptr;
            cb();
        }
    }
}

// ── Easing ────────────────────────────────────────────────────────────────────

float Fadeable::applyTween(float t, Tween tween)
{
    switch (tween)
    {
        case EXPONENTIAL:
            // Ease-out: fast start, decelerates toward 1
            return 1.f - std::pow(1.f - t, 3.f);

        case LINEAR:
        default:
            return t;
    }
}

} // namespace ml
