
#include <Malena/Engine/App/Application.h>

namespace ml
{
	Application::Application(const sf::VideoMode &videoMode, const std::string &title, UIController &appLogic,
							 sf::RenderWindow &window) : AppManager(videoMode, title, appLogic, window)
	{
	}

	Application::Application(unsigned int screenWidth, unsigned int screenHeight, unsigned int bitDepth,
		const std::string &title) : Application(sf::VideoMode({screenWidth, screenHeight}, bitDepth), title, *this)
	{
	}

	Application::Application(const sf::VideoMode &videoMode, const std::string &title)
		: Application(videoMode, title, *this)
	{
	}

	void Application::addComponent(Core &component)
	{
		CoreManager<Core>::addComponent(component);
	}

} // namespace ml
