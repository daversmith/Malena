#ifndef APPLICATION_H
#define APPLICATION_H

#include <Malena/Engine/App/UIController.h>
#include <Malena/Engine/App/AppManager.h>
#include <Malena/Core/CoreManager.h>
#include <string>
#pragma once

namespace ml
{
	class Application : public AppManager, public UIController
	{
	public:
		Application(const sf::VideoMode &videoMode, const std::string &title, UIController &appLogic,
					sf::RenderWindow &window = WindowManager::getWindow());
		Application(unsigned int screenWidth, unsigned int screenHeight, unsigned int bitDepth, const std::string& title);
		Application(const sf::VideoMode &videoMode, const std::string &title);

		void addComponent(Core &component);
		// using UIManager::UIManager;
	};
} // namespace ml
#endif // APPLICATION_H
