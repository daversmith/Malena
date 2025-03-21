// //
// // Created by Dave Smith on 3/11/25.
// //
//
// #include "UIManagerEDA.h"
// //
// // Created by Dave Smith on 3/11/25.
// //
//
// #include "UIManagerMVC.h"
// //
// // Created by Dave R. Smith on 3/5/25.
// //
//
// #include "UIManagerMVC.h"
// #include "EventsManager.h"
//
// namespace ml {
//
// UIManagerEDA::UIManagerEDA(const sf::VideoMode &videoMode, const std::string &title, UIController& appLogic, sf::RenderWindow &window)
//         : window(&window)
// {
//         this->window->create(videoMode,  title);
//         this->window->setFramerateLimit(60);
//         UIManagerEDA::appLogic = &appLogic;
// }
//
// void UIManagerEDA::handleEvents()
// {
//         for(auto &c : appLogic->getUIComponents())
//         {
//                 c->eventHandler();
//         }
// }
// void UIManagerEDA::draw()
// {
//         this->window->clear();
//         for(auto &c : appLogic->getUIComponents())
//         {
//                 this->window->draw(*c);
//         }
//
//         this->window->display();
// }
//
// void UIManagerEDA::updateComponents()
// {
//
//         for(auto &c : appLogic->getUIComponents())
//                 c->update();
//         EventsManager::fire("update");
// }
//
// void UIManagerEDA::run()
// {
//         appLogic->initialization();
//         appLogic->registerEvents();
//         // UIManagerEDA::appLogic->addComponent(proxy);
//         // proxy.onUpdate([](){});
//         while(this->window->isOpen())
//         {
//                 while(const std::optional event = this->window->pollEvent())
//                 {
//                         if(event->is<sf::Event::Closed>())
//                         {
//                                 this->window->close();
//                                 return;
//                         }
//                         handleEvents();
//                         fireEvents(event);
//                 }
//
//                 updateComponents();
//                 for(auto &c : appLogic->getUIComponents())
//                         c->update();
//                 draw();
//         }
//
// }
//
//
//
// void UIManagerEDA::fireEvents(const std::optional<sf::Event> &event)
// {
//       if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
//       {
//               EventsManager::fire("click", [this](UIComponent& c)
//               {
//                 return MouseEvents::isClicked(c, *window);
//               });
//       }
//         if(event->is<sf::Event::MouseMoved>())
//         {
//               EventsManager::fire("hover", [this](UIComponent& c)
//               {
//                 return MouseEvents::isHovered(c, *window);
//               });
//         }
//
// }
//
// // void UIManagerEDA::onUpdate(std::function<void()> f)
// // {
// //         proxy.onUpdate(f);
// // }
// } // namespace ml
//
