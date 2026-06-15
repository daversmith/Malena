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
    // linkChild is the legacy free-function spelling; it now funnels through the
    // single insertion path (addComponent) so registration, layer ordering and
    // the enable cascade behave identically no matter which spelling a caller
    // uses. Previously linkChild skipped setParentEnabled, leaving children
    // added this way with a stale enabled state.
    void Core::linkChild(Core* parent, Core* child)
    {
        if (!parent || !child || parent == child) return;
        parent->addComponent(*child);
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

        // Insert if new, or reposition if already ours at a different layer.
        // Either way the vector stays sorted via positional insert — no O(n log n)
        // re-sort on every registration.
        auto it = std::find_if(_children.begin(), _children.end(),
            [&child](const Child& e) { return e.component == &child; });
        if (it == _children.end())
        {
            insertChildSorted(child, layer);
        }
        else if (it->layer != layer)
        {
            _children.erase(it);
            insertChildSorted(child, layer);
        }

        child.setParentEnabled(isEnabled());
    }

    void Core::insertChildSorted(Core& child, int layer)
    {
        // First entry whose layer is strictly greater than ours; inserting there
        // places this child after all same-or-lower layers, preserving
        // registration order within a layer.
        auto pos = std::upper_bound(_children.begin(), _children.end(), layer,
            [](int lyr, const Child& e) { return lyr < e.layer; });
        _children.insert(pos, {&child, layer});
        child._parent = this;
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

    // ── Hit testing ─────────────────────────────────────────────────────────
    Core* Core::topmostMatching(const std::function<bool(Core&)>& accept)
    {
        // Children are painted over us, so they win ties — visit them first,
        // highest layer (last painted) first. _children is sorted ascending, so
        // iterate in reverse. Recurse depth-first so a grandchild on top beats
        // its parent.
        for (auto it = _children.rbegin(); it != _children.rend(); ++it)
        {
            Core* child = it->component;
            if (!child || !child->isVisible()) continue;
            if (Core* hit = child->topmostMatching(accept))
                return hit;
        }
        // Then us, painted beneath our children.
        if (accept(*this)) return this;
        return nullptr;
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
