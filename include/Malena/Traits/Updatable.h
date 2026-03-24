//
// Updatable.h
//

#pragma once
#include <Malena/Engine/Events/Fireable.h>
#include <Malena/Engine/Events/EventReceiver.h>

#include <Malena/Engine/Events/FrameDispatcher.h>

namespace ml
{
	/**
	 * @brief Receiver trait for per-frame and window lifecycle events.
	 * @ingroup Traits
	 *
	 * Inherit through @c Core. @c onUpdate fires every frame.
	 * Window events fire when the application window changes state.
	 */
	class Updatable : public EventReceiver
	{
	public:
		// ── Per-frame ─────────────────────────────────────────────────────────
		void onUpdate(std::function<void()> f);
		void onUpdate(std::function<void(const std::optional<sf::Event>&)> f);

		// ── Window events ─────────────────────────────────────────────────────
		void onWindowResized(std::function<void()> f);
		void onWindowResized(std::function<void(const std::optional<sf::Event>&)> f);

		void onWindowFocusGained(std::function<void()> f);
		void onWindowFocusGained(std::function<void(const std::optional<sf::Event>&)> f);

		void onWindowFocusLost(std::function<void()> f);
		void onWindowFocusLost(std::function<void(const std::optional<sf::Event>&)> f);
	};

	/**
	 * @brief Dispatcher singleton for per-frame and window lifecycle events.
	 * @ingroup Traits
	 *
	 * Fires @c "update" every frame and window events when they occur.
	 * Registered via @c ML_EXPORT(UpdatableDispatcher).
	 */
	class UpdatableDispatcher : public FrameDispatcher
	{
	public:
		// UpdatableDispatcher() : FrameDispatcher(){}
		// bool occurred(const std::optional<sf::Event>& event) override { return true; }
		bool filter(const std::optional<sf::Event> &event, Core *component) override { return true; }
		// void fire(const std::optional<sf::Event>& event) override;
		void fire() override;

		bool occurred() override{return true;};
	};

} // namespace ml

ML_EXPORT(UpdatableDispatcher)