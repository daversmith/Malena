//
// Created by Dave Smith on 10/4/25.
//

#include <Malena/Core/Core.h>
#include <algorithm>

namespace ml
{

    std::unordered_map<Core*, std::vector<Core*>> Core::_childMap;

    bool Core::isDescendantOf(Core* ancestor, Core* component)
    {
        if (!ancestor || !component) return false;
        auto it = _childMap.find(ancestor);
        if (it == _childMap.end()) return false;
        for (Core* child : it->second)
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
        Core::unlinkAll(this);
    }

    void Core::linkChild(Core* parent, Core* child)
    {
        if (!parent || !child || parent == child) return;
        auto& children = _childMap[parent];
        if (std::find(children.begin(), children.end(), child) == children.end())
            children.push_back(child);
    }

    void Core::unlinkAll(Core* core)
    {
        _childMap.erase(core);
        for (auto& [p, children] : _childMap)
            children.erase(std::remove(children.begin(), children.end(), core), children.end());
    }

    void Core::addComponent(Core& child)
    {
        linkChild(this, &child);
        child.setParentEnabled(isEnabled());
    }

    void Core::applyEnabled(bool selfEnabled, bool parentEnabled)
    {
        _selfEnabled   = selfEnabled;
        _parentEnabled = parentEnabled;
        bool effective = _selfEnabled && _parentEnabled;
        setFlag(Flag::ENABLED, effective);
        onEnabledChanged(effective);
        auto it = _childMap.find(this);
        if (it != _childMap.end())
            for (auto* child : it->second)
                child->setParentEnabled(effective);
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
