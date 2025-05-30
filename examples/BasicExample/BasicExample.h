#ifndef TEXTINPUTTEST_H
#define TEXTINPUTTEST_H

#pragma once

#include <Malena/common.hpp>


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
