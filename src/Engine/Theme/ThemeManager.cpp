// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Traits/Theme/Themeable.h>
#include <Malena/Resources/ThemeManager.h>

namespace ml
{
	// ── Reading ───────────────────────────────────────────────────────────────

	const Theme& ThemeManager::get()
	{
		return *_active;
	}

	// ── Applying themes ───────────────────────────────────────────────────────

	void ThemeManager::applyTheme(const Theme& theme)
	{
		_active = std::make_unique<Theme>(theme);
		notify();
	}

	// ── Subscription ─────────────────────────────────────────────────────────

	void ThemeManager::subscribe(Themeable* component)
	{
		if (_destroyed) return;

		deferOrExecute([component]{
			if (_destroyed) return;
			if (std::find(_subscribers.begin(),
						  _subscribers.end(), component) == _subscribers.end())
				_subscribers.push_back(component);
		});
	}

	void ThemeManager::unsubscribe(Themeable* component)
	{
		if (_destroyed) return;

		deferOrExecute([component]{
			if (_destroyed) return;
			_subscribers.erase(
				std::remove(_subscribers.begin(),
							_subscribers.end(), component),
				_subscribers.end());
		});
	}

	// ── Lifecycle ─────────────────────────────────────────────────────────────

	void ThemeManager::shutdown()
	{
		_destroyed = true;
		clearPending();       // discard any queued subscribe/unsubscribe ops
		_subscribers.clear();
	}

	// ── Notify ───────────────────────────────────────────────────────────────

	void ThemeManager::notify()
	{
		if (_destroyed) return;

		beginBusy();
		for (Themeable* t : _subscribers)
			t->onThemeApplied(*_active);
		endBusy(); // flushes any subscribe/unsubscribe calls deferred during iteration
	}

} // namespace ml