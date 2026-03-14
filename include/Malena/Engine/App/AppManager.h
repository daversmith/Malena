//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef UIMANAGER_H
#define UIMANAGER_H

#pragma once

#include <SFML/Graphics.hpp>
#include <Malena/Engine/App/UIController.h>
#include <Malena/Core/Manager.h>
#include <Malena/Core/CoreManager.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <string>
namespace ml
{

	class AppManager : public Manager
	{
	private:
		UIController *uiController;
		sf::RenderWindow *window = nullptr;

	public:
		enum Architecture
		{
			MVC,
			EDA,
			ECS
		};


		AppManager(const sf::VideoMode &videoMode, const std::string &title, UIController &appLogic,
				  sf::RenderWindow &window = WindowManager::getWindow(), Architecture architechture = MVC);

	private:
		void fireInputEvents(const std::optional<sf::Event> &event) override;

		void fireUpdateEvents() override;

		void draw() override;

	public:
		void run() override;

		// void onUpdate(std::function<void()>) override;


		virtual ~AppManager() = default;
	};


} // namespace ml
#endif // UIMANAGER_H
