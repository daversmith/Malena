#ifndef APPLICATION_H
#define APPLICATION_H

#include "../Controllers/UIController.h"
#include "../Managers/UIManager.h"

#pragma once

namespace ml
{
	class Application : public UIManager, public UIController
	{
	public:
		Application(const sf::VideoMode &videoMode, const std::string &title, UIController &appLogic,
					sf::RenderWindow &window = WindowManager::getWindow());
		Application(unsigned int screenWidth, unsigned int screenHeight, unsigned int bitDepth, const std::string& title);
		Application(const sf::VideoMode &videoMode, const std::string &title);

		void addComponent(UIComponent &component) override;
		void addToApplication(UIComponent &component);
		// using UIManager::UIManager;
	};
} // namespace ml
#endif // APPLICATION_H
