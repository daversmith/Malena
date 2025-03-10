#include <SFML/Graphics.hpp>
#include "../examples/RectangleApp.h"
#include "Graphics/Button.h"
#include "Graphics/Circle.h"
#include "Graphics/Grid.h"
#include "Graphics/RectangleButton.h"

int main()
{
    RectangleApp app;
    Rectangle r;
    r.setSize({50.f, 50.f});
    auto &font = FontManager::getFont(FontManager::ARIAL);
    RectangleButton r1(font,sf::Vector2f({50.f, 50.f}), "1"),
    r2(FontManager::DEFAULT,sf::Vector2f({50.f, 50.f}), "2"), r3(font,sf::Vector2f({50.f, 50.f}), "3");
    Circle r4(25.f), r5(25.f), r6(25.f);
    Grid grid({3, 2});

    grid.addComponent(r1);
    grid.addComponent(r2);
    grid.addComponent(r3);
    grid.addComponent(r4);
    grid.addComponent(r5);
    grid.addComponent(r6);


    r.onClick([&r]()
    {
       // r.setSize({300.f, 300.f});
    });

    r.onUpdate([&r]()
    {
        // if(r.checkState(StateManager::CLICKED))
            r.move({0, .05f});
    });

    app.onUpdate([&r]()
    {
        if(r.getPosition().y > 200)
            r.setSize({300.f, 300.f});
    });

    app.addComponent(grid);
    // app.addComponent(r);
    app.run();
}
//     #include <SFML/Graphics.hpp>
// #include <vector>
// #include <iostream>
// #include <cmath>
// #include <random>
//
// using namespace std;
// using namespace sf;
//
// // Window settings
// const unsigned int WINDOW_WIDTH = 800;
// const unsigned int WINDOW_HEIGHT = 600;
// const unsigned int PLATFORM_WIDTH = 100;
// const unsigned int PLATFORM_HEIGHT = 20;
// const unsigned int PLAYER_SIZE = 30;
// const float GRAVITY = 0.5f;
//
// // Struct for BST Node
// struct PlatformNode {
//     int value;
//     Vector2f position;
//     PlatformNode* left;
//     PlatformNode* right;
//
//     PlatformNode(int val, Vector2f pos) : value(val), position(pos), left(nullptr), right(nullptr) {}
// };
//
// // Function to insert into a BST
// PlatformNode* insert(PlatformNode* root, int val, float x, float y, float offsetX) {
//     if (!root) return new PlatformNode(val, Vector2f(x, y));
//
//     if (val < root->value)
//         root->left = insert(root->left, val, x - offsetX, y + 80, offsetX / 2);
//     else
//         root->right = insert(root->right, val, x + offsetX, y + 80, offsetX / 2);
//
//     return root;
// }
//
// // Function to create a balanced BST
// PlatformNode* createBalancedBST(vector<int>& values, int start, int end, float x, float y, float offsetX) {
//     if (start > end) return nullptr;
//
//     int mid = (start + end) / 2;
//     PlatformNode* node = new PlatformNode(values[mid], Vector2f(x, y));
//
//     node->left = createBalancedBST(values, start, mid - 1, x - offsetX, y + 80, offsetX / 2);
//     node->right = createBalancedBST(values, mid + 1, end, x + offsetX, y + 80, offsetX / 2);
//
//     return node;
// }
//
// // Function to draw the BST (Platforms)
// void drawBST(RenderWindow& window, PlatformNode* root, Font& font) {
//     if (!root) return;
//
//     // Draw platform (rectangle)
//     RectangleShape platform(Vector2f(PLATFORM_WIDTH, PLATFORM_HEIGHT));
//     platform.setFillColor(Color::Blue);
//     platform.setPosition(root->position);
//     window.draw(platform);
//
//     // Draw node value
//     Text text(font);
//     text.setString(to_string(root->value));
//     text.setCharacterSize(14);
//     text.setFillColor(Color::White);
//     text.setPosition({root->position.x + 40, root->position.y - 5});
//     window.draw(text);
//
//     // Draw connections
//     if (root->left) {
//         Vertex line[] = {
//             Vertex({root->position + Vector2f(PLATFORM_WIDTH / 2,
//                 PLATFORM_HEIGHT), Color::White}),
//             Vertex({root->left->position + Vector2f(PLATFORM_WIDTH / 2, 0),
//                 Color::White})
//         };
//         window.draw(line, 2, sf::PrimitiveType::Lines);
//         drawBST(window, root->left, font);
//     }
//     if (root->right) {
//         Vertex line[] = {
//             Vertex({root->position + Vector2f(PLATFORM_WIDTH / 2, PLATFORM_HEIGHT),
//                 Color::White}),
//             Vertex({root->right->position + Vector2f(PLATFORM_WIDTH / 2, 0),
//                 Color::White})
//         };
//         window.draw(line, 2, sf::PrimitiveType::Lines);
//         drawBST(window, root->right, font);
//     }
// }
//
// // Player struct
// struct Player {
//     CircleShape shape;
//     Vector2f velocity;
//     PlatformNode* currentPlatform;
//
//     Player(PlatformNode* startPlatform) {
//         shape.setRadius(PLAYER_SIZE / 2);
//         shape.setFillColor(Color::Red);
//         shape.setPosition({startPlatform->position.x + 35,
//             startPlatform->position.y - PLAYER_SIZE});
//         velocity = {0, 0};
//         currentPlatform = startPlatform;
//     }
//
//     void update() {
//         // Gravity
//         velocity.y += GRAVITY;
//
//         // Apply velocity
//         shape.move(velocity);
//
//         // Prevent falling through the platform
//         if (shape.getPosition().y + PLAYER_SIZE >= currentPlatform->position.y &&
//             shape.getPosition().x >= currentPlatform->position.x &&
//             shape.getPosition().x <= currentPlatform->position.x + PLATFORM_WIDTH) {
//             shape.setPosition({shape.getPosition().x,
//                 currentPlatform->position.y - PLAYER_SIZE});
//             velocity.y = 0;
//         }
//     }
//
//     void moveLeft() {
//         if (currentPlatform->left) {
//             currentPlatform = currentPlatform->left;
//             shape.setPosition({currentPlatform->position.x + 35,
//                 currentPlatform->position.y - PLAYER_SIZE});
//         }
//     }
//
//     void moveRight() {
//         if (currentPlatform->right) {
//             currentPlatform = currentPlatform->right;
//             shape.setPosition({currentPlatform->position.x + 35,
//                 currentPlatform->position.y - PLAYER_SIZE});
//         }
//     }
// };
//
// // Main game function
// int main() {
//     RenderWindow window(VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Binary Tree Platformer");
//
//     // Generate a balanced BST with values
//     vector<int> values = {1, 2, 3, 4, 5, 6, 7};
//     PlatformNode* root = createBalancedBST(values, 0, values.size() - 1, WINDOW_WIDTH / 2, 50, 200);
//
//     // Select a random leaf node for player start position
//     PlatformNode* startPlatform = root;
//     while (startPlatform->left || startPlatform->right) {
//         if (startPlatform->left && rand() % 2) startPlatform = startPlatform->left;
//         else if (startPlatform->right) startPlatform = startPlatform->right;
//     }
//
//     // Player
//     Player player(startPlatform);
//
//     // Load font
//     Font font;
//     if (!font.openFromFile("fonts/arial.ttf")) {
//         cerr << "Failed to load font!" << endl;
//         return -1;
//     }
//
//     while (window.isOpen()) {
//         while (std::optional event = window.pollEvent()) {
//             if (event->is<Event::Closed>())
//                 window.close();
//         }
//
//         // Player movement
//         if (Keyboard::isKeyPressed(Keyboard::Key::Left)) player.moveLeft();
//         if (Keyboard::isKeyPressed(Keyboard::Key::Right)) player.moveRight();
//
//         player.update();
//
//         // Render everything
//         window.clear();
//         drawBST(window, root, font);
//         window.draw(player.shape);
//         window.display();
//     }
//
//     return 0;
// }
// #include <SFML/Graphics.hpp>
// #include <SFML/Window.hpp>
// #include <iostream>
// #include <string>
//
// using namespace sf;
// using namespace std;
//
// // Define a Binary Tree Node for Attack Decisions
// struct AttackNode {
//     string attackName;
//     AttackNode* left;
//     AttackNode* right;
//
//     AttackNode(string name) : attackName(name), left(nullptr), right(nullptr) {}
// };
//
// // Enemy AI that selects attacks using a BST
// class EnemyAI {
// private:
//     AttackNode* root;
//     AttackNode* currentAttack;
//
// public:
//     EnemyAI() {
//         // Construct attack tree
//         root = new AttackNode("Mid Attack");
//         root->left = new AttackNode("Weak Attack");
//         root->right = new AttackNode("Strong Attack");
//
//         root->left->left = new AttackNode("EZ Attack");
//         root->left->right = new AttackNode("Medium Attack");
//
//         root->right->left = new AttackNode("Hard Attack");
//         root->right->right = new AttackNode("Ultra Attack");
//
//         currentAttack = root;
//     }
//
//     // Decide next attack based on player's performance
//     void decideNextAttack(bool playerDodged) {
//         if (playerDodged && currentAttack->left) {
//             currentAttack = currentAttack->left;
//         } else if (!playerDodged && currentAttack->right) {
//             currentAttack = currentAttack->right;
//         }
//     }
//
//     // Get the current attack move
//     string getCurrentAttack() {
//         return currentAttack->attackName;
//     }
//
//     // Reset the attack sequence
//     void reset() {
//         currentAttack = root;
//     }
// };
//
// // Main Game Loop
// int main() {
//     RenderWindow window(VideoMode({600, 400}), "Binary Tree Boss Battle");
//
//     Font font;
//     if (!font.openFromFile("fonts/arial.ttf")) {
//         cerr << "Failed to load font!" << endl;
//         return -1;
//     }
//
//     // Text elements for UI
//     Text attackText(font), instructionText(font);
//     attackText.setFont(font);
//     attackText.setCharacterSize(24);
//     attackText.setFillColor(Color::White);
//     attackText.setPosition({150, 100});
//
//     instructionText.setFont(font);
//     instructionText.setCharacterSize(18);
//     instructionText.setFillColor(Color::Yellow);
//     instructionText.setPosition({50, 250});
//     instructionText.setString("Press LEFT to Dodge (Weaker Attack) \nPress RIGHT to Take Hit (Stronger Attack)");
//
//     EnemyAI enemyAI;
//
//     while (window.isOpen()) {
//         while (std::optional event = window.pollEvent()) {
//             if (event->is<Event::Closed>()) {
//                 window.close();
//             }
//         }
//
//         // Player inputs determine the AI attack path
//         if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
//             enemyAI.decideNextAttack(true);  // Dodged → go left (weaker)
//         } else if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
//             enemyAI.decideNextAttack(false); // Hit → go right (stronger)
//         }
//
//         // Update attack display
//         attackText.setString("Enemy Attack: " + enemyAI.getCurrentAttack());
//
//         // Render game
//         window.clear();
//         window.draw(attackText);
//         window.draw(instructionText);
//         window.display();
//     }
//
//     return 0;
// }
