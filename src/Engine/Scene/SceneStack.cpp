// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

//
// SceneStack.cpp
//

#include <Malena/Engine/Scene/SceneStack.h>
#include <Malena/Engine/Scene/SceneLifecycle.h>
#include <Malena/Engine/Events/EventManager.h>
#include <algorithm>

namespace ml
{
    namespace
    {
        SceneLifecycle* hooks(Core* scene)
        {
            return dynamic_cast<SceneLifecycle*>(scene);
        }
    }

    SceneStack::SceneStack()
    {
        _live.push_back(this);

        // Changes requested while an event was being dispatched wait here,
        // after the handler that asked for them has returned.
        onUpdate([this]() { applyPending(); });
    }

    SceneStack::~SceneStack()
    {
        _live.erase(std::remove(_live.begin(), _live.end(), this), _live.end());

        // No lifecycle hooks during teardown: the app may already be closing.
        _pending.clear();
        while (!_layers.empty())
        {
            removeComponent(*_layers.back().scene);
            _layers.pop_back();
        }
    }

    // ── Changes ─────────────────────────────────────────────────────────────

    void SceneStack::push(std::unique_ptr<Core> scene, Mode mode)
    {
        if (!scene) return;
        request({Change::Type::Push, std::move(scene), mode});
    }

    void SceneStack::pop(std::size_t count)
    {
        for (std::size_t i = 0; i < count; ++i)
            request({Change::Type::Pop, nullptr});
    }

    void SceneStack::replace(std::unique_ptr<Core> scene, Mode mode)
    {
        if (!scene) return;
        request({Change::Type::Replace, std::move(scene), mode});
    }

    void SceneStack::clear()
    {
        request({Change::Type::Clear, nullptr});
    }

    void SceneStack::request(Change change)
    {
        _pending.push_back(std::move(change));

        // Inside event dispatch the requesting handler may belong to a scene
        // this change destroys, so wait for the next update. A change made
        // from a lifecycle hook joins the batch that is already applying.
        if (!_applying && !EventManager::isBusy())
            applyPending();
    }

    void SceneStack::applyPending()
    {
        if (_applying || _pending.empty()) return;
        _applying = true;

        // refresh() can fire onEnabledChanged, which may request more changes.
        do
        {
            while (!_pending.empty())
            {
                Change change = std::move(_pending.front());
                _pending.pop_front();
                apply(change);
            }
            refresh();
        } while (!_pending.empty());

        _applying = false;
    }

    void SceneStack::apply(Change& change)
    {
        switch (change.type)
        {
            case Change::Type::Push:
                pushLayer(std::move(change.scene), change.mode, true);
                break;

            case Change::Type::Pop:
                popLayer(true);
                break;

            case Change::Type::Replace:
                // The scene underneath stays paused: it is covered before and after.
                popLayer(false);
                pushLayer(std::move(change.scene), change.mode, false);
                break;

            case Change::Type::Clear:
                while (!_layers.empty())
                    popLayer(false);
                break;
        }
    }

    void SceneStack::pushLayer(std::unique_ptr<Core> scene, Mode mode, bool pauseBelow)
    {
        if (pauseBelow && !_layers.empty())
            if (auto* h = hooks(_layers.back().scene.get()))
                h->onScenePause();

        Core& added = *scene;
        addComponent(added, _nextLayer++);
        _layers.push_back({std::move(scene), mode, added.isEnabled(), added.isVisible()});

        if (auto* h = hooks(&added))
            h->onSceneEnter();
    }

    void SceneStack::popLayer(bool resumeBelow)
    {
        if (_layers.empty()) return;

        Core* scene = _layers.back().scene.get();
        if (auto* h = hooks(scene))
            h->onSceneExit();

        removeComponent(*scene);
        std::unique_ptr<Core> doomed = std::move(_layers.back().scene);
        _layers.pop_back();
        doomed.reset();

        if (_layers.empty())
            _nextLayer = 0;
        else if (resumeBelow)
            if (auto* h = hooks(_layers.back().scene.get()))
                h->onSceneResume();
    }

    void SceneStack::refresh()
    {
        // Walk top to bottom. The top scene is live; the rest are disabled.
        // A scene is drawn while every scene above it is an overlay.
        bool shown = true;
        for (std::size_t i = _layers.size(); i-- > 0;)
        {
            Layer& layer = _layers[i];
            const bool isTop = i + 1 == _layers.size();

            // Only touch what changed, so controls on an unchanged scene
            // don't get a spurious onEnabledChanged.
            if (layer.enabled != isTop)
            {
                layer.enabled = isTop;
                layer.scene->setEnabled(isTop);
            }
            if (layer.visible != shown)
            {
                layer.visible = shown;
                layer.scene->setVisible(shown);
            }

            shown = shown && layer.mode == Mode::Overlay;
        }
    }

    // ── Query ───────────────────────────────────────────────────────────────

    Core* SceneStack::top() const
    {
        return _layers.empty() ? nullptr : _layers.back().scene.get();
    }

    std::size_t SceneStack::size() const
    {
        return _layers.size();
    }

    bool SceneStack::empty() const
    {
        return _layers.empty();
    }

    bool SceneStack::hasPendingChanges() const
    {
        return !_pending.empty();
    }

    SceneStack* SceneStack::of(const Core& scene)
    {
        for (auto* stack : _live)
            if (stack->holds(scene))
                return stack;
        return nullptr;
    }

    bool SceneStack::holds(const Core& scene) const
    {
        for (const auto& layer : _layers)
            if (layer.scene.get() == &scene)
                return true;
        for (const auto& change : _pending)
            if (change.scene.get() == &scene)
                return true;
        return false;
    }

} // namespace ml
