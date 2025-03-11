//
// Created by Dave R. Smith on 3/10/25.
//

#include "JeopardyModel.h"

JeopardyModel::JeopardyModel()
{
}

JeopardyModel::JeopardyModel(const std::string &filename, sf::Vector2f gridSize, sf::Vector2u gridDimensions,
  sf::Vector2f questionWindowSize)
{
}
sf::Vector2f JeopardyModel::get_grid_size() const
{
  return gridSize;
}

void JeopardyModel::set_grid_size(const sf::Vector2f &grid_size)
{
  gridSize = grid_size;
}

sf::Vector2u JeopardyModel::get_grid_dimensions() const
{
  return gridDimensions;
}

void JeopardyModel::set_grid_dimensions(const sf::Vector2u &grid_dimensions)
{
  gridDimensions = grid_dimensions;
}

sf::Vector2f JeopardyModel::get_question_window_size() const
{
  return questionWindowSize;
}

void JeopardyModel::set_question_window_size(const sf::Vector2f &question_window_size)
{
  questionWindowSize = question_window_size;
}

float JeopardyModel::get_spacing() const
{
  return spacing;
}

void JeopardyModel::load_questions(const std::string &filename)
{
  std::ifstream fin(filename);
  std::vector<std::pair<std::string, std::string>> q;
  if(!fin.fail())
  {
    std::string question;
    while(getline(fin, question))
    {
      std::string answer;
      getline(fin, answer);
      q.push_back({question, answer});
    }
    fin.close();
    std::random_device rd;
    std::mt19937 rng(rd());  // Mersenne Twister PRNG

    // Shuffle the vector
    std::shuffle(q.begin(), q.end(), rng);
    for(auto & x : q)
      questions.push(x);
  }
  else
    throw "Could not open file";
}
void JeopardyModel::set_filename(const std::string &filename)
{
  load_questions(filename);
}

bool JeopardyModel::has_more_questions()
{
  return !questions.empty();
}

std::pair<std::string, std::string> JeopardyModel::get_next_question()
{
  auto p = questions.top();
  questions.pop();
  return p;
}
