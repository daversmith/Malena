//
// Created by Dave Smith on 3/27/26.
//

#ifndef UICONTROLS_H
#define UICONTROLS_H


#include <Malena/Engine/App/Application.h>

#include <Malena/Graphics/Controls/List.h>
#include <Malena/Graphics/Controls/ScrollPane.h>

#include "Malena/Graphics/Controls/RadioButton.h"


class UIControls : public  ml::Application
{
private:
	ml::ScrollPane pane;
	ml::List list, list2;
	ml::RadioButton radio;
	void setupScrollPane();
	void setupLists();
	void setupRadio();
public:
	UIControls();

};



#endif //UICONTROLS_H
