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
        for (Core* child : ancestor->_children)
        {
            if (child == component) return true;
            if (isDescendantOf(child, component)) return true;
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
        if (std::find(v.begin(), v.end(), child) != v.end()) return;

        v.push_back(child);
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
        std::vector<Core*> kids;
        kids.swap(core->_children);
        for (Core* c : kids)
            if (c) c->_parent = nullptr;
    }

    void Core::doRemoveChild(Core* c)
    {
        if (!c) return;

        if (_iterDepth > 0)
        {
            _pendingOps.emplace_back([this, c]{ doRemoveChild(c); });
            return;
        }

        auto it = std::find(_children.begin(), _children.end(), c);
        if (it == _children.end()) return;

        if ((*it)->_parent == this)
            (*it)->_parent = nullptr;
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
        linkChild(this, &child);
        child.setParentEnabled(isEnabled());
    }

    void Core::removeComponent(Core& child)
    {
        doRemoveChild(&child);
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
        for (Core* c : _children)
            c->setParentEnabled(effective);
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
