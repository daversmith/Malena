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
	// Single-winner: only the front-most component that WANTS clicks (resolved in
	// fire()) receives it. Everything painted behind it — and any decorative /
	// full-window container with no real handler in front of it — is transparent.
	return component != nullptr && component == _clickTarget;
}

void ml::Clickable::onClick(std::function<void()> f, bool overwrite)
{
	_wantsClick = true;
	EventCallback cb = [f = std::move(f)](const std::optional<sf::Event>&){ f(); };
	Fireable::addCallback(Event::CLICK, this, std::move(cb), overwrite);
}

void ml::Clickable::onClick(std::function<void(const std::optional<sf::Event>&)> f, bool overwrite)
{
	_wantsClick = true;
	Fireable::addCallback(Event::CLICK, this, std::move(f), overwrite);
}

void ml::Clickable::subscribeClickPassive()
{
	// Stay present in the CLICK channel with an empty handler, but do NOT mark
	// this component as wanting clicks — so it never wins a click and behaves as
	// click-transparent until a real onClick is registered.
	Fireable::addCallback(Event::CLICK, this,
		[](const std::optional<sf::Event>&){}, /*overwrite*/ true);
}

namespace {
	// Walk the app's top-level components front-to-back (last registered is
	// painted in front) and return the front-most descendant satisfying accept.
	ml::Core* frontMost(const std::function<bool(ml::Core&)>& accept)
	{
		const auto& roots = ml::AppManager::get().getComponents();
		for (auto it = roots.rbegin(); it != roots.rend(); ++it)
		{
			ml::Core* root = *it;
			if (!root || !root->isVisible()) continue;
			if (ml::Core* hit = root->topmostMatching(accept))
				return hit;
		}
		return nullptr;
	}
}

void ml::ClickableDispatcher::fire(const std::optional<sf::Event>& event)
{
	// Click receipt → the front-most component under the cursor that WANTS clicks.
	_clickTarget = frontMost([](Core& c){ return passesClickGate(&c) && c.wantsClick(); });

	// Focus → the front-most gated component under the cursor, independent of
	// wanting clicks, so a focus-only widget (e.g. TextInput, which has no real
	// onClick) still focuses. Decoupling focus from click receipt is what lets an
	// empty-handler overlay be click-transparent while real targets still focus.
	_focusTarget = frontMost([](Core& c){ return passesClickGate(&c); });

	if (_focusTarget != _focused)
	{
		if (_focused)
		{
			_focused->disableFlag(Flag::FOCUSED);
			if (auto* f = dynamic_cast<Focusable*>(_focused)) f->process(Event::BLUR, event);
		}
		if (_focusTarget)
		{
			_focusTarget->enableFlag(Flag::FOCUSED);
			if (auto* f = dynamic_cast<Focusable*>(_focusTarget)) f->process(Event::FOCUS, event);
		}
		_focused = _focusTarget;
	}

	// Fire the click handler for the single winner. Focus is already handled
	// above, so no resolve/reject callbacks are needed here.
	EventManager::fire(Event::CLICK, this, event);
}
