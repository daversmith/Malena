//
// Created by Dave R. Smith on 3/5/25.
//

#include <Malena/Engine/App/AppManager.h>

#include <Malena/Engine/Events/Fireable.h>
#include <Malena/Engine/Events/EventManager.h>

namespace ml
{
    AppManager::AppManager(const sf::VideoMode &videoMode, const std::string &title,
                         sf::RenderWindow &window) : window(&window)
    {
        window.create(videoMode, title);
        this->window->setFramerateLimit(60);
    }

    void AppManager::draw()
    {
    	_isDrawing = true;
        this->window->clear();
        for (auto &c : CoreManager<Core>::getComponents())
        {
        	if (!c->checkFlag(Flag::HIDDEN))
        	{
        		auto* drawable = dynamic_cast<sf::Drawable*>(c);
        		if (drawable)
        			window->draw(*drawable, c->getRenderStates());
        	}
        }
        this->window->display();
    	_isDrawing = false;
    }

    void AppManager::fireUpdateEvents()
    {
        for (auto* dispatcher : Fireable::_fireables_frame)
        {
            if (dispatcher->occurred())
                dispatcher->fire();
        }
    }

    void AppManager::run()
    {
        onInit();
    	onReady();
        while (this->window->isOpen())
        {
            while (const std::optional event = this->window->pollEvent())
            {
                if (event->is<sf::Event::Closed>())
                {
                    this->window->close();
                    return;
                }
                fireInputEvents(event);
            }

            fireUpdateEvents();
            draw();
        }
        delete window;
    }

    void AppManager::fireInputEvents(const std::optional<sf::Event> &event)
    {
    	for (auto* dispatcher : Fireable::_fireables)
    	{
    		if (dispatcher->occurred(event))
    		{
    			dispatcher->fire(event);
    		}

    	}
    	// while (event);
    }
} // namespace ml