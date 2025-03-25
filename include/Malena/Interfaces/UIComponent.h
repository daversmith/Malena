#ifndef UICOMPONENT_H
#define UICOMPONENT_H


#include <SFML/Graphics.hpp>

#include <iostream>
#include "../Managers/WindowManager.h"
#include "../Traits/Messenger.h"
#include "../Traits/Positionable.h"
#include "../Utilities/MouseEvents.h"
#include "Component.h"
#include "Updateable.h"

namespace ml
{

	class UIComponent : public virtual sf::Drawable,
						public Messenger,
						public Component,
						public Stateful,
						public Positionable
	{
	protected:
		sf::RenderWindow *window = nullptr;
		bool isDynamic = false;

	public:
		explicit UIComponent(sf::RenderWindow &window = WindowManager::getWindow());
		[[nodiscard]] virtual sf::FloatRect getGlobalBounds() const = 0;
		virtual ~UIComponent();
		void *operator new(size_t size)
		{
			UIComponent *obj = static_cast<UIComponent *>(malloc(size));
			obj->isDynamic = true; // Mark as dynamically allocated
			return obj;
		}
		friend class ComponentsManager;
	};


} // namespace ml
#endif // UICOMPONENT_H
