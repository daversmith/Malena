#ifndef UICOMPONENT_H
#define UICOMPONENT_H


#include <SFML/Graphics.hpp>

#include <iostream>
#include <Malena/Interfaces/UIComponent.h>
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
	template<typename Manifest = void>
	class UIComponentWith : public virtual UIComponent,
							public CustomFlaggable<typename extract_Flags<Manifest>::type>,
							public StateManager<typename extract_State<Manifest>::type>
	{
	public:
		// Bring both enableFlag overloads into scope
		using UIComponent::enableFlag;
		using UIComponent::disableFlag;
		using UIComponent::setFlag;
		using UIComponent::toggleFlag;
		using UIComponent::checkFlag;

		template<typename T = typename extract_Flags<Manifest>::type>
		std::enable_if_t<!std::is_void_v<T>>
		enableFlag(T flag) {
			this->CustomFlaggable<typename extract_Flags<Manifest>::type>::enableFlag(flag);
		}
		template<typename T = typename extract_Flags<Manifest>::type>
			std::enable_if_t<!std::is_void_v<T>>
			disableFlag(T flag) {
			this->CustomFlaggable<typename extract_Flags<Manifest>::type>::disableFlag(flag);
		}
		template<typename T = typename extract_Flags<Manifest>::type>
		std::enable_if_t<!std::is_void_v<T>, bool>
		checkFlag(T flag) const {
			return this->CustomFlaggable<typename extract_Flags<Manifest>::type>::checkFlag(flag);
		}

		// State methods - wrapper instead of using declaration
		template<typename T = typename extract_State<Manifest>::type>
		std::enable_if_t<!std::is_void_v<T>>
		setFlag(T flag, bool value) {
			this->CustomFlaggable<typename extract_Flags<Manifest>::type>::setFlagFlag(flag, value);
		}

		template<typename T = typename extract_Flags<Manifest>::type>
		std::enable_if_t<!std::is_void_v<T>>
		toggleFlag(T flag) {
			this->CustomFlaggable<typename extract_Flags<Manifest>::type>::toggleFlag(flag);
		}

	};



} // namespace ml
#endif // UICOMPONENT_H
