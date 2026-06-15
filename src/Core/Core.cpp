//
// Created by Dave Smith on 10/4/25.
//

#include <Malena/Core/Core.h>
#include <algorithm>

namespace ml
{
    // ── Subtree query ───────────────────────────────────────────────────────
    bool Core::isDescendantOf(Core* ancestor, Core* component)
    {
        if (!ancestor || !component) return false;
        for (const Child& entry : ancestor->_children)
        {
            if (entry.component == component) return true;
            if (isDescendantOf(entry.component, component)) return true;
        }
        return false;
    }

    Core::Core()
    {
        enableFlag(Flag::ENABLED);
    }

    Core::~Core()
    {
        EventManager::forceUnsubscribeAll(this);

        // Detach from parent — Core::unlinkAll handles both directions safely
        // even when called from within destruction.
        Core::unlinkAll(this);
    }

    // ── Topology mutations ──────────────────────────────────────────────────
    void Core::linkChild(Core* parent, Core* child)
    {
        if (!parent || !child || parent == child) return;

        // Soft single-parent: silently move the child if it already belongs to
        // a different parent. Preserves the implicit invariant that every
        // historical caller relied on (each ListItem owns its own row, each
        // Modal owns its own content, etc).
        if (child->_parent && child->_parent != parent)
            child->_parent->doRemoveChild(child);

        // De-dup: do not add the same child twice to the same parent.
        auto& v = parent->_children;
        auto it = std::find_if(v.begin(), v.end(),
            [child](const Child& e) { return e.component == child; });
        if (it != v.end()) return;

        v.push_back({child, DefaultLayer});
        // No sort needed — appending an entry at DefaultLayer to a vector
        // where the most recent insert was also at DefaultLayer keeps it
        // sorted. addComponent(child, layer) re-sorts when a non-default
        // layer would break the invariant.
        std::stable_sort(v.begin(), v.end(),
            [](const Child& a, const Child& b) { return a.layer < b.layer; });
        child->_parent = parent;
    }

    void Core::unlinkAll(Core* core)
    {
        if (!core) return;

        // Detach from parent.
        if (core->_parent)
        {
            Core* p = core->_parent;
            core->_parent = nullptr;            // clear first to avoid recursion
            p->doRemoveChild(core);
        }

        // Orphan any remaining children — they may outlive us. Use a local copy
        // so children destructing during this loop can safely null their own
        // _parent without invalidating our iteration.
        std::vector<Child> kids;
        kids.swap(core->_children);
        for (Child& e : kids)
            if (e.component) e.component->_parent = nullptr;
    }

    void Core::doRemoveChild(Core* c)
    {
        if (!c) return;

        if (_iterDepth > 0)
        {
            _pendingOps.emplace_back([this, c]{ doRemoveChild(c); });
            return;
        }

        auto it = std::find_if(_children.begin(), _children.end(),
            [c](const Child& e) { return e.component == c; });
        if (it == _children.end()) return;

        if (it->component->_parent == this)
            it->component->_parent = nullptr;
        _children.erase(it);
    }

    void Core::runPendingIfDepthZero()
    {
        if (_iterDepth != 0) return;
        // Move-out to allow ops to enqueue further ops without growing the
        // vector we are draining.
        std::vector<std::function<void()>> ops;
        ops.swap(_pendingOps);
        for (auto& op : ops) op();
    }

    void Core::addComponent(Core& child)
    {
        addComponent(child, DefaultLayer);
    }

    void Core::addComponent(Core& child, int layer)
    {
        // Soft single-parent: move from previous parent if any.
        if (child._parent && child._parent != this)
            child._parent->doRemoveChild(&child);

        // Update layer if already a child of ours; otherwise insert.
        auto it = std::find_if(_children.begin(), _children.end(),
            [&child](const Child& e) { return e.component == &child; });
        if (it == _children.end())
        {
            _children.push_back({&child, layer});
            child._parent = this;
        }
        else
        {
            it->layer = layer;
        }

        // Stable sort: layer ascending, registration order preserved within
        // a layer.
        std::stable_sort(_children.begin(), _children.end(),
            [](const Child& a, const Child& b) { return a.layer < b.layer; });

        child.setParentEnabled(isEnabled());
    }

    void Core::removeComponent(Core& child)
    {
        doRemoveChild(&child);
    }

    // ── Children rendering ─────────────────────────────────────────────────
    void Core::drawChildren(sf::RenderTarget& target, sf::RenderStates states) const
    {
        for (const Child& entry : _children)
        {
            if (!entry.component->isVisible()) continue;
            if (const auto* d = dynamic_cast<const sf::Drawable*>(entry.component))
                target.draw(*d, states);
        }
    }

    // ── Enable cascade ──────────────────────────────────────────────────────
    void Core::applyEnabled(bool selfEnabled, bool parentEnabled)
    {
        _selfEnabled   = selfEnabled;
        _parentEnabled = parentEnabled;
        const bool effective = _selfEnabled && _parentEnabled;
        setFlag(Flag::ENABLED, effective);
        onEnabledChanged(effective);

        // Walk children with a reentrancy guard. A child's setParentEnabled may
        // fire onEnabledChanged which may in turn call removeComponent on us;
        // those mutations defer until the cascade unwinds.
        ++_iterDepth;
        for (Child& e : _children)
            e.component->setParentEnabled(effective);
        --_iterDepth;
        runPendingIfDepthZero();
    }

    void Core::setEnabled(bool enabled)
    {
        applyEnabled(enabled, _parentEnabled);
    }

    void Core::setParentEnabled(bool enabled)
    {
        applyEnabled(_selfEnabled, enabled);
    }

    void Core::setVisible(bool visible)
    {
        setFlag(Flag::HIDDEN, !visible);
    }

    void Core::setActive(bool active)
    {
        setVisible(active);
        setEnabled(active);
    }

    bool Core::isEnabled() const
    {
        return checkFlag(Flag::ENABLED);
    }

    bool Core::isVisible() const
    {
        return !checkFlag(Flag::HIDDEN);
    }

} // namespace ml
