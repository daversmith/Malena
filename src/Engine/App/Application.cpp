
#include <Malena/Engine/App/Application.h>

#include "Malena/Engine/Messaging/MessageManager.h"
#include "Malena/Resources/ThemeManager.h"

namespace ml
{
    ApplicationBase::ApplicationBase(const sf::VideoMode& videoMode,
                                     const std::string& title,
                                     sf::RenderWindow& window,
                                     AppManager::Architecture architecture,
                                     std::uint32_t windowStyle)
        : AppManager(videoMode, title, window, architecture, windowStyle)
    {
    }

    ApplicationBase::ApplicationBase(unsigned int screenWidth,
                                     unsigned int screenHeight,
                                     unsigned int bitDepth,
                                     const std::string& title,
                                     std::uint32_t windowStyle)
        : ApplicationBase(sf::VideoMode({screenWidth, screenHeight}, bitDepth),
                          title,
                          WindowManager::getWindow(),
                          AppManager::MVC,
                          windowStyle)
    {
    }

    void ApplicationBase::addComponent(Core& component)
    {
        CoreManager<Core>::addComponent(component);
    }

    void ApplicationBase::clearEvents()
    {
        EventManager::clear();
    }

    void ApplicationBase::reset()
    {
        EventManager::clear();
        MessageManager::clear();
        CoreManager<Core>::clear();
    }

    ApplicationBase::~ApplicationBase()
    {
        ml::ThemeManager::shutdown();
    }

} // namespace ml
