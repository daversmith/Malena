// //
// // Created by Dave Smith on 3/11/25.
// //
//
// #ifndef UIMANAGERMVC_H
// #define UIMANAGERMVC_H
//
// #include <vector>
// #include <SFML/Graphics.hpp>
// #include "WindowManager.h"
// #include "../Controllers/UIController.h"
// #include "../Adapters/UIComponentAdapter.h"
// #include "Manager.h"
//
// namespace ml {
// class UIManagerMVC : public Manager
// {
// private:
//     // std::vector<Component *> components;
//     UIController *appLogic;
//     sf::RenderWindow *window = nullptr;
//     //handle events
//     void handleEvents();
//     void fireEvents(const std::optional<sf::Event> &event);
//     // void firePublishedEvents() const;
//     //update objects
//     void updateComponents();
//     UIComponentAdapter proxy;
//     //draw
//         void draw();
// public:
//     virtual ~UIManagerMVC() = default;
//
//     UIManagerMVC(const sf::VideoMode& videoMode, const std::string& title, UIController& appLogic,
//               sf::RenderWindow &window = WindowManager::window);
//
//     void run();
//
//     // void onUpdate(std::function<void()>);
// };
//
//
// } // namespace ml
// #endif //UIMANAGERMVC_H
//
//
