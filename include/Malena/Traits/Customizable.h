#ifndef UICOMPONENT_H
#define UICOMPONENT_H


#include <SFML/Graphics.hpp>

#include <iostream>
#include <Malena/Traits/CustomFlaggable.h>
#include <Malena/Managers/StateManager.h>
namespace ml
{


	template<typename T, typename = void>
	struct extract_Flags { using type = void; };

	template<typename T>
	struct extract_Flags<T, std::void_t<typename T::Flags>> {
		using type = typename T::Flags;
	};

	template<typename T, typename = void>
	struct extract_State { using type = void; };

	template<typename T>
	struct extract_State<T, std::void_t<typename T::State>> {
		using type = typename T::State;
	};

	// Use them in UIComponentWith
	template<typename Component, typename Manifest = void>
	class Customizable :	public Component,
					public CustomFlaggable<typename extract_Flags<Manifest>::type>,
					public StateManager<typename extract_State<Manifest>::type>
	{
	public:

		Customizable() : Component() {}

		// Forwarding constructor with SFINAE to avoid ambiguity
		template<typename Arg, typename... Args,
			 typename = std::enable_if_t<!std::is_same_v<std::decay_t<Arg>, Customizable>>>
		explicit Customizable(Arg&& arg, Args&&... args)
		    : Component(std::forward<Arg>(arg), std::forward<Args>(args)...)
		{}
		// using Component::Component;
		// Bring both enableFlag overloads into scope
		// using Component::enableFlag;
		// using Component::disableFlag;
		// using Component::setFlag;
		// using Component::toggleFlag;
		// using Component::checkFlag;

		template<typename T = typename extract_Flags<Manifest>::type>
		void enableFlag(T flag) {
			this->CustomFlaggable<typename extract_Flags<Manifest>::type>::enableFlag(flag);
		}

		template<typename T = typename extract_Flags<Manifest>::type>
		void disableFlag(T flag) {
			this->CustomFlaggable<typename extract_Flags<Manifest>::type>::disableFlag(flag);
		}
		template<typename T = typename extract_Flags<Manifest>::type>
		bool checkFlag(T flag) const {
			return this->CustomFlaggable<typename extract_Flags<Manifest>::type>::checkFlag(flag);
		}

		// State methods - wrapper instead of using declaration
		template<typename T = typename extract_Flags<Manifest>::type>
		void setFlag(T flag, bool value) {
			this->CustomFlaggable<typename extract_Flags<Manifest>::type>::setFlag(flag, value);
		}

		template<typename T = typename extract_Flags<Manifest>::type>
		void toggleFlag(T flag) {
			this->CustomFlaggable<typename extract_Flags<Manifest>::type>::toggleFlag(flag);
		}

	};


} // namespace ml
#endif // UICOMPONENT_H
