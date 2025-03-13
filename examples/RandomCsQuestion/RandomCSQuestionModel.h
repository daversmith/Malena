//
// Created by Dave R. Smith on 3/11/25.
//

#ifndef RANDOMCSQUESTIONMODEL_H
#define RANDOMCSQUESTIONMODEL_H

#include <string>
#include <stack>
#include <fstream>
#include <vector>

#include "SFML/Graphics/Font.hpp"

class RandomCSQuestionModel {

private:
  std::stack<std::string> stack;
  std::string filename;
  std::string buttonName;
  sf::Font font;
  sf::Vector2u buttonSize = {100, 50};
public:
  RandomCSQuestionModel();
  const std::string& getNextQuestion();
  void loadQuestions(const std::string& filename);
  void setButtonName(const std::string& buttonName);
  const std::string& getButtonName() const;
  const sf::Font& getFont() const;
  const sf::Vector2u getButtonSize() const;
};



#endif //RANDOMCSQUESTIONMODEL_H
