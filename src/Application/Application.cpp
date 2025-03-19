
#include "Application.h"

namespace ml {
//
// Created by Dave Smith on 3/7/25.
//

    Application::Application(const sf::VideoMode &videoMode, const std::string &title, UIController& appLogic, sf::RenderWindow &window)
    : UIManager(videoMode, title, appLogic, window)
    {
    }
} // namespace ml

