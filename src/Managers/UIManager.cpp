
//
// Created by Dave R. Smith on 3/5/25.
//


#include <Malena/Managers/UIManager.h>

namespace ml
{
	UIManager::UIManager(const sf::VideoMode &videoMode, const std::string &title, UIController &uiController,
						 sf::RenderWindow &window, Architecture archType) : uiController(&uiController), window(&window)
	{
		window.create(videoMode, title);
		this->window->setFramerateLimit(60);
	}

	void UIManager::draw()
	{
		this->window->clear();
		for (auto &c : uiController->getUIComponents())
		{
			if (!c->checkFlag(Flag::HIDDEN))
				c->draw(*window, c->getRenderStates());
			// 	//this->window->draw(*c);
		}

		this->window->display();
	}

	void UIManager::fireUpdateEvents()
	{
		EventsManager::fire("update");
	}

	void UIManager::run()
	{
		uiController->initialization();
		uiController->registerEvents();
		// uiController->addComponent(proxy);
		// proxy.onUpdate([](){});
		while (this->window->isOpen())
		{
			std::optional<sf::Event> *evPtr;
			while (const std::optional event = this->window->pollEvent())
			{
				if (event->is<sf::Event::Closed>())
				{
					this->window->close();

					return;
				}
				fireInputEvents(event);
			}

			fireUpdateEvents();
			draw();
		}
	}


	void UIManager::fireInputEvents(const std::optional<sf::Event> &event)
	{
		if (event->is<sf::Event::MouseButtonPressed>() ||
		event->is<sf::Event::MouseButtonReleased>() ||
		event->is<sf::Event::MouseMoved>())
		{
			EventsManager::fire("draggable", nullptr, nullptr, event);
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			EventsManager::fire(
					"click",
					[this, &event](UIComponent &c) -> bool {
						bool isClicked = MouseEvents::isClicked(c, *window);
						bool isFocused = c.checkFlag(Flag::FOCUSED);

						if (isClicked)
						{
							if (!isFocused)
							{
								c.enableFlag(Flag::FOCUSED);

								EventsManager::fire(
										"focus", [this, &c](UIComponent &comp) -> bool { return (&comp == &c); },
										nullptr, event);
							}
						}
						else
						{
							if (isFocused)
							{
								EventsManager::fire(
										"blur", [this, &c](UIComponent &comp) -> bool { return (&comp == &c); },
										nullptr, event);
								c.disableFlag(Flag::FOCUSED);
							}
						}
						return isClicked;
					},
					nullptr, event);
		}

		/// Checking Event (use else-if)
		if (event->is<sf::Event::MouseMoved>())
		{
			EventsManager::fire(
					"hover",
					[this, &event](UIComponent &c) {
						bool isHovered = MouseEvents::isHovered(c, *window);
						if (isHovered)
							c.enableFlag(ml::Flag::HOVERED);
						else
						{
							c.disableFlag(ml::Flag::HOVERED);
							EventsManager::fire(
									"unhover", [&event](UIComponent &c) { return !c.checkFlag(Flag::HOVERED); },
									nullptr, event);
						}

						return isHovered;
					},
					nullptr, event);
		}
		else if (event->is<sf::Event::TextEntered>())
		{
			EventsManager::fire("textentered", nullptr, nullptr, event);
		}
		else if (event->is<sf::Event::MouseMoved>())
		{
			EventsManager::fire("mousemoved", nullptr, nullptr, event);
		}
		else if (event->is<sf::Event::KeyPressed>())
		{
			EventsManager::fire("keypress", nullptr, nullptr, event);
		}
	}
} // namespace ml
