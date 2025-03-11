//
// Created by Dave Smith on 3/7/25.
//
#include "Application.h"
Application::Application(const sf::VideoMode &videoMode, const std::string &title, AppController& appLogic, sf::RenderWindow &window)
: UIManager(videoMode, title, appLogic, window)
{
}

