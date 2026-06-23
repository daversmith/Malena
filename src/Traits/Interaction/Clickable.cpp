//
// Created by Dave Smith on 3/20/26.
//
#include <Malena/Traits/Interaction/Clickable.h>

#include <Malena/Core/Core.h>
#include <Malena/Engine/App/AppManager.h>
#include <Malena/Engine/Events/EventManager.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <Malena/Utilities/MouseEvents.h>


bool ml::ClickableDispatcher::occurred(const std::optional<sf::Event> &event)
{
	if (event->is<sf::Event::MouseButtonReleased>())
	{
		if (event->getIf<sf::Event::MouseButtonReleased>()->button == sf::Mouse::Button::Left)
			return true;
	}
	return false;
}
bool ml::ClickableDispatcher::passesClickGate(Core *component)
{
	auto* positionable = dynamic_cast<Positionable*>(component);
	if (!positionable) return false;
	// Effective visibility, not the bare HIDDEN flag: a component inside a
	// hidden container (e.g. a collapsed Accordion section) is not drawn and
	// must not be hit-testable either, even though its own flag says visible.
	if (!component->isEffectivelyVisible() || !component->checkFlag(Flag::ENABLED)) return false;
	if (!AppManager::isUnderExclusiveOwner(component)) return false;
	return MouseEvents::isHovered(*positionable, WindowManager::getWindow());
}

bool ml::ClickableDispatcher::filter(const std::optional<sf::Event> &/*event*/, Core *component)
{
	// Topmost-wins: only the front-most gated component (chosen in fire())
	// receives the click. Everything painted behind it is rejected, so clicks
	// no longer fall through overlapping widgets.
	return component != nullptr && component == _topTarget;
}

void ml::Clickable::onClick(std::function<void()> f, bool overwrite)
{
	EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
	Fireable::addCallback(Event::CLICK, this, std::move(cb), overwrite);
}

void ml::Clickable::onClick(std::function<void(const std::optional<sf::Event>&)> f, bool overwrite)
{
	Fireable::addCallback(Event::CLICK, this, std::move(f), overwrite);
}
void ml::ClickableDispatcher::fire(const std::optional<sf::Event>& event)
{
	// Resolve the single front-most gated component under the cursor. Top-level
	// components draw in registration order, so the last one is painted in front;
	// walk them front-to-back and, within each, let topmostMatching find the
	// front-most descendant that passes the click gate. The first hit wins.
	_topTarget = nullptr;
	const auto& roots = AppManager::get().getComponents();
	for (auto it = roots.rbegin(); it != roots.rend(); ++it)
	{
		Core* root = *it;
		if (!root || !root->isVisible()) continue;
		if (Core* hit = root->topmostMatching([](Core& c){ return passesClickGate(&c); }))
		{
			_topTarget = hit;
			break;
		}
	}

	EventManager::fire(Event::CLICK, this, event,
		[this](EventReceiver* component, const std::optional<sf::Event>& e)
		{
			auto* core = dynamic_cast<Core*>(component);
			if (!core) return;

			// If something else was focused, blur it first
			if (_focused && _focused != component)
			{
				auto* focusedCore = dynamic_cast<Core*>(_focused);
				if (focusedCore) focusedCore->disableFlag(Flag::FOCUSED);

				auto* focusable = dynamic_cast<Focusable*>(_focused);
				if (focusable) focusable->process(Event::BLUR, e);

				_focused = nullptr;
			}

			// Focus this component if not already focused
			if (!core->checkFlag(Flag::FOCUSED))
			{
				core->enableFlag(Flag::FOCUSED);

				auto* focusable = dynamic_cast<Focusable*>(core);
				if (focusable) focusable->process(Event::FOCUS, e);

				_focused = component;
			}
		},
		[this](EventReceiver* component, const std::optional<sf::Event>& e)
		{
			// Clicked elsewhere — blur if focused
			if (component == _focused)
			{
				auto* core = dynamic_cast<Core*>(component);
				if (core) core->disableFlag(Flag::FOCUSED);

				auto* focusable = dynamic_cast<Focusable*>(component);
				if (focusable) focusable->process(Event::BLUR, e);

				_focused = nullptr;
			}
		});
}
