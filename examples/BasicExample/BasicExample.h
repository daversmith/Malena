#ifndef TEXTINPUTTEST_H
#define TEXTINPUTTEST_H
#include <melena/common.hpp> // Changed from "../../Malena/common.hpp"

class BasicExample : public ml::Application
{
	ml::RectangleButton box1, box2, myEventButton, unsubscribeBtn, subscribeBtn, unsubscribeAll, clearAll;
	ml::Typer typer;

public:
	BasicExample();
	void initialization() override;
	void registerEvents() override;
};

#endif // TEXTINPUTTEST_H
