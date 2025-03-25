//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef UIMANAGER_H
#define UIMANAGER_H

#pragma once

#include <SFML/Graphics.hpp>
#include "../Controllers/UIController.h"
#include "Manager.h"
#include "UIManagerEDA.h"
#include "UIManagerMVC.h"
#include "WindowManager.h"

namespace ml
{

	class UIManager : public Manager
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


		UIManager(const sf::VideoMode &videoMode, const std::string &title, UIController &appLogic,
				  sf::RenderWindow &window = WindowManager::getWindow(), Architecture architechture = MVC);

	private:
		void fireInputEvents(const std::optional<sf::Event> &event) override;

		void fireUpdateEvents() override;

		void draw() override;

	public:
		void run() override;

		// void onUpdate(std::function<void()>) override;


		virtual ~UIManager() = default;
	};


} // namespace ml
#endif // UIMANAGER_H
