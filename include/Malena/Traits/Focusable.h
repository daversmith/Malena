//
// Created by Dave Smith on 3/21/26.
//

#ifndef FOCUSABLE_H
#define FOCUSABLE_H
#include "Malena/Engine/Events/EventReceiver.h"

namespace ml {


	class Focusable : public EventReceiver
	{
		public:
		void onFocus(std::function<void()> f);
		void onFocus(std::function<void(const std::optional<sf::Event>&)> f);
		void onBlur(std::function<void()> f);
		void onBlur(std::function<void(const std::optional<sf::Event>&)> f);
	};
} // ml

#endif //FOCUSABLE_H
