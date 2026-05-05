//
// Created by Dave Smith on 10/4/25.
//

#include <Malena/Core/Core.h>
namespace ml
{

    Core::Core()
    {
        enableFlag(Flag::ENABLED);
    }

    Core::~Core()
    {
        EventManager::forceUnsubscribeAll(this);
    }

    void Core::setEnabled(bool enabled)
    {
        setFlag(Flag::ENABLED, enabled);
        onEnabledChanged(enabled);
    }

    void Core::setVisible(bool visible)
    {
        setFlag(Flag::HIDDEN, !visible);
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
