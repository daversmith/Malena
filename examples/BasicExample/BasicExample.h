#ifndef TEXTINPUTTEST_H
#define TEXTINPUTTEST_H

#pragma once

#include <Malena/common.hpp>

#include "BasicExampleManifest.h"


class BasicExample : public ml::ApplicationWith<BasicExampeManifest>
{
	ml::RectangleButton box1, box2, myEventButton, unsubscribeBtn, subscribeBtn, unsubscribeAll, clearAll;
	ml::Circle circle;
	ml::Typer typer;

	enum Event {MY_EVENT};
public:
	BasicExample();
	void initialization() override;
	void registerEvents() override;
};

#endif // TEXTINPUTTEST_H
