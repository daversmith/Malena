//
// Created by Dave R. Smith on 3/10/25.
//

#ifndef JEORPADRYMODEL_H
#define JEORPADRYMODEL_H

#include <fstream>
#include <stack>
#include <vector>
#include <algorithm>  // For std::shuffle
#include <random>
#include <SFML/Graphics.hpp>

class JeopardyModel {
  std::stack<std::pair<std::string, std::string>> questions;
  sf::Vector2f gridSize = {720, 420};
  sf::Vector2u gridDimensions = {5, 6};
  sf::Vector2f questionWindowSize = {400, 200};
  float spacing = 5;

public:
  [[nodiscard]] float get_spacing() const;

private:
  void load_questions(const std::string &filename);
public:
  JeopardyModel();
  JeopardyModel(const std::string& filename, sf::Vector2f gridSize,
    sf::Vector2u gridDimensions, sf::Vector2f questionWindowSize);

  [[nodiscard]] sf::Vector2f get_grid_size() const;

  void set_grid_size(const sf::Vector2f &grid_size);

  [[nodiscard]] sf::Vector2u get_grid_dimensions() const;

  void set_grid_dimensions(const sf::Vector2u &grid_dimensions);

  [[nodiscard]] sf::Vector2f get_question_window_size() const;

  void set_question_window_size(const sf::Vector2f &question_window_size);
  void set_filename(const std::string& filename);
  bool has_more_questions();
  std::pair<std::string, std::string> get_next_question();
};



#endif //JEORPADRYMODEL_H
