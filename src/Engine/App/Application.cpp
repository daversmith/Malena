
#include <Malena/Engine/App/Application.h>

namespace ml
{
	ApplicationBase::ApplicationBase(const sf::VideoMode &videoMode, const std::string &title, UIController &appLogic,
							 sf::RenderWindow &window) : AppManager(videoMode, title, appLogic, window)
	{
	}

	ApplicationBase::ApplicationBase(unsigned int screenWidth, unsigned int screenHeight, unsigned int bitDepth,
		const std::string &title) : ApplicationBase(sf::VideoMode({screenWidth, screenHeight}, bitDepth), title, *this)
	{
	}

	ApplicationBase::ApplicationBase(const sf::VideoMode &videoMode, const std::string &title)
		: ApplicationBase(videoMode, title, *this)
	{
	}

	void ApplicationBase::addComponent(Core &component)
	{
		CoreManager<Core>::addComponent(component);
	}

	void ApplicationBase::clearEvents()
	{
		EventsManager::clear();
	}

	void ApplicationBase::reset()
	{
		EventsManager::clear();
		MessageManager::clear();
		CoreManager<Core>::clear();
	}
} // namespace ml
