//
// Created by Dave Smith on 3/8/25.
//

#ifndef CONTROLLER_H
#define CONTROLLER_H

#pragma once


namespace ml
{
	class Controller
	{
	public:
		virtual void initialization() = 0;

		virtual void registerEvents() = 0;

		virtual ~Controller()  = default;
	};
} // namespace ml

#endif // CONTROLLER_H
