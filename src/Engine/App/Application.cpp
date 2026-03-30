
#include <Malena/Engine/App/Application.h>

#include "Malena/Resources/ThemeManager.h"

namespace ml
{
	ApplicationBase::ApplicationBase(const sf::VideoMode &videoMode, const std::string &title,
							 sf::RenderWindow &window) : AppManager(videoMode, title, window)
	{
	}

	ApplicationBase::ApplicationBase(unsigned int screenWidth, unsigned int screenHeight, unsigned int bitDepth,
		const std::string &title) : ApplicationBase(sf::VideoMode({screenWidth, screenHeight}, bitDepth), title)
	{
	}


	void ApplicationBase::addComponent(Core &component)
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
