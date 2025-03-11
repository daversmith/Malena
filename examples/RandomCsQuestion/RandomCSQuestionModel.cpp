//
// Created by Dave R. Smith on 3/11/25.
//

#include "RandomCSQuestionModel.h"

RandomCSQuestionModel::RandomCSQuestionModel()
{
  if(!font.openFromFile("fonts/arial.ttf"))
    throw "Cannot load font";
}

const sf::Font & RandomCSQuestionModel::getFont() const
{
  return font;
}

const std::string & RandomCSQuestionModel::getNextQuestion()
{
  if(!stack.empty())
  {
    std::string question = stack.top();
    stack.pop();
    return question;
  }
  throw "Stack is empty";
}

void RandomCSQuestionModel::loadQuestions(const std::string &filename)
{
  std::ifstream fin(filename);
  if(!fin.fail())
  {
    std::vector<std::string> strings;
    std::string next;
    while(getline(fin, next))
      strings.push_back(next);

    for(int i=0; i<strings.size(); i++)
    {
      int r = rand() % strings.size();
      swap(strings[i], strings[r]);
    }
    fin.close();
    for(auto & i : strings)
      stack.push(i);
  }
  else
    throw "File not found";
}

void RandomCSQuestionModel::setButtonName(const std::string &buttonName)
{
  RandomCSQuestionModel::buttonName = buttonName;
}

const std::string & RandomCSQuestionModel::getButtonName() const
{
  return buttonName;
}

const sf::Vector2u RandomCSQuestionModel::getButtonSize() const
{
  return buttonSize;
}
