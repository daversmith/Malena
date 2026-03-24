//
// Created by Dave Smith on 3/20/26.
//

#include "../../../include/Malena/Engine/Events/Fireable.h"

#include <iostream>

#include "Malena/Engine/Events/EventsManager.h"

#include <Malena/Core/Core.h>


ml::Fireable::Fireable(DispatchType type) : _type(type)
{
}

void ml::Fireable::_unregister(Fireable* f)
{
	_fireables.erase(
		std::remove(_fireables.begin(), _fireables.end(), f),
		_fireables.end());
	_fireables_frame.erase(
		std::remove(_fireables_frame.begin(), _fireables_frame.end(), f),
		_fireables_frame.end());

}

ml::Fireable::Cleanup::~Cleanup()
{
	// for (auto* f : Fireable::_fireables)
	// 	delete f;
	Fireable::_fireables.clear();
	// for (auto* f : Fireable::_fireables_frame)
	// 	delete f;
	Fireable::_fireables_frame.clear();
}

void ml::Fireable::_register(Fireable *f)
{
	if (f->_type == DispatchType::FRAME)
		_fireables_frame.push_back(f);
	else
	_fireables.push_back(f);
}

