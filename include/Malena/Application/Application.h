#ifndef APPLICATION_H
#define APPLICATION_H

#include <Malena/Controllers/UIController.h>
#include <Malena/Managers/UIManager.h>
#include <Malena/Managers/ComponentsManager.h>
#include <string>
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

		void addComponent(UIComponent &component);
		// using UIManager::UIManager;
	};
} // namespace ml
#endif // APPLICATION_H
