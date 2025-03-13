#include <SFML/Graphics.hpp>
// #include "../ECS/ECSManager.h"
// #include "../ECS/EventManagerECS.h"
// #include "../ECS/HealthSystem.h"
// #include "../ECS/MovementSystem.h"
// #include "../ECS/RenderingSystem.h"
#include "../examples/Jeopardy/Jeopardy.h"

int main()
{
    Jeopardy j;
    j.run();
    return 0;
}
// int main()
// {
//   sf::RenderWindow window(sf::VideoMode({800, 600}, 32), "ECS with Event-Driven Health");
//   window.setFramerateLimit(60);
//
//   ECSManager ecs;
//   EventManagerECS eventManager;
//   MovementSystem movementSystem;
//   HealthSystem healthSystem;
//   RenderingSystem renderingSystem;
//
//   // Create entities
//   Entity player = 1;
//   ecs.positions[player] = {100, 100};
//   ecs.velocities[player] = {50, 0};  // Moving right
//   ecs.healths[player] = {100};  // Player starts with 100 HP
//   ecs.size[player] = {5};
//
//   Entity player2 = 2;
//   ecs.positions[player2] = {10, 10};
//   ecs.velocities[player2] = {25, 0};  // Moving right
//   ecs.healths[player2] = {100};  // Player starts with 100 HP
//   ecs.size[player2] = {10};
//   // for(int i=0; i<1000; i++)
//   // {
//   //   float x = rand()%100;
//   //   float y = rand()%100;
//   //   float vx = rand()%500;
//   //   float vy = rand()%500;
//   //   int size = rand() % 10;
//   //   ecs.positions[i] = {x, y};
//   //   ecs.velocities[i] = {vx, vy};  // Moving right
//   //   ecs.healths[i] = {100};  // Player starts with 100 HP
//   //   ecs.size[i] = {size};
//   //
//   // }
//
//   while (window.isOpen()) {
//
//     while (std::optional event = window.pollEvent()) {
//       if ( event->is<sf::Event::Closed>()) {
//         window.close();
//       }
//       if (event->is<sf::Event::KeyPressed>()) {
//         if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Space) {
//           // Press SPACE to apply damage
//           healthSystem.applyDamage(eventManager, ecs, player, 20);
//         }
//       }
//     }
//
//     // Process all queued events (e.g., apply damage)
//     eventManager.processEvents();
//
//     // Update & render
//     movementSystem.update(ecs, 0.016f);  // 16ms per frame
//     window.clear();
//     renderingSystem.render(window, ecs);
//     window.display();
//   }
//
//   return 0;
// }