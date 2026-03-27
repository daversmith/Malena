//
// Created by Dave Smith on 3/27/26.
//

#include "UIControls.h"

#include "Malena/Graphics/Controls/ListItem.h"
UIControls::UIControls() : ml::Application(720, 420, 32, "UI Controls Test"), pane(300, 200)
{
	setupLists();
	setupScrollPane();
	setupRadio();
}
void UIControls::setupScrollPane()
{
	pane.addComponent(list);
	pane.addComponent(list2);
	pane.setPosition({100, 100});
	addComponent(pane);
}
void UIControls::setupLists()
{


	auto a = new ml::ListItem("This is a test");
	auto b = new ml::ListItem("This is a test");
	auto c = new ml::ListItem("This is a test");
	// todo: figure out wht set size needs to be called inorder for things to position correctly
	a->setSize({500, 50});
	b->setSize({500, 50});
	c->setSize({500, 50});
	list.addItem(a);
	list.addItem(b);
	list.addItem(c);

	auto x = new ml::ListItem("This is a test");
	auto y = new ml::ListItem("This is a test");
	auto z = new ml::ListItem("This is a test");
	x->setSize({500, 50});
	y->setSize({500, 50});
	z->setSize({500, 50});
	list2.addItem(x);
	list2.addItem(y);
	list2.addItem(z);
	list.addItem(&list2);
}
void UIControls::setupRadio()
{
	radio.setLabel("This is a radio");
	radio.setBelow(pane, 30.f);
	addComponent(radio);
}
