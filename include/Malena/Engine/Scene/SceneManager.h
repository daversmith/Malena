// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

//
// SceneManager.h
//

#ifndef MALENA_SCENEMANAGER_H
#define MALENA_SCENEMANAGER_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Core.h>
#include <Malena/Utilities/EnumClassHash.h>
#include <unordered_map>
#include <functional>
#include <vector>
#include <memory>
#include <type_traits>

namespace ml
{
    /**
     * @brief State-driven scene router for Malena state machines.
     * @ingroup EngineApp
     *
     * @c SceneManager<StateEnum> binds scenes (any @c Core-derived object or
     * lazily-constructed type) to state enum values. When the owner calls
     * @c setState(), @c SceneManager intercepts the transition via
     * @c onStateEnter / @c onStateExit and performs the scene swap automatically —
     * removing the outgoing scene from the owner and adding the incoming one.
     *
     * ### Ownership
     * A @c SceneManager is a plain object you own — typically a member of the
     * thing that drives it. That owner can be an @c Application **or** any
     * @c Component, so the same router works at the top level of an app and for
     * sub-routing *inside* a component. Each instance has its own bindings and
     * history; the router lives and dies with its owner.
     *
     * The owner (the type passed to @c attach) must expose:
     * - @c onStateEnter(std::function<void(StateEnum)>)
     * - @c onStateExit(std::function<void(StateEnum)>)
     * - @c setState(StateEnum)
     * - @c syncState(StateEnum)
     * - @c addComponent(Core&)
     * - @c removeComponent(Core&)
     *
     * @c Application and @c Component both satisfy this contract.
     *
     * @warning The owner must outlive the @c SceneManager. @c attach() stores
     *          callbacks that reference the owner and @c this; destroying either
     *          while still attached is undefined behaviour. Holding the
     *          @c SceneManager as a member of the owner satisfies this naturally.
     *
     * ### Design contract
     * - @c setState() on the owner is the **only** navigation primitive.
     *   @c SceneManager reacts to it; it does not replace it.
     * - @c back() is the one exception — it calls @c setState() with the
     *   previously visited state, keeping the state machine as the single
     *   source of truth.
     * - Scenes bound by reference (@c bind) are always in memory. Scenes bound
     *   lazily (@c bindLazy) are constructed on first visit and destroyed on
     *   @c back() or when another scene replaces them.
     *
     * ### Typical setup (Application)
     * @code
     * // AppManifest.h
     * struct AppManifest : public ml::Manifest
     * {
     *     enum class State { MainMenu, Game, Settings };
     * };
     *
     * // MyApp.h
     * class MyApp : public ml::ApplicationWith<AppManifest>
     * {
     *     using State  = AppManifest::State;
     *     using Scenes = ml::SceneManager<State>;
     *
     *     Scenes        _scenes;     // owned router
     *     MainMenuScene _mainMenu;   // eager — always in memory
     *     SettingsScene _settings;   // eager
     *     // GameScene is lazy — only allocated when visited
     *
     * public:
     *     MyApp() : ml::ApplicationWith<AppManifest>(1280, 720, 32, "My Game") {}
     *
     *     void onInit() override
     *     {
     *         _scenes.bind    (State::MainMenu,  _mainMenu);
     *         _scenes.bind    (State::Settings,  _settings);
     *         _scenes.bindLazy<GameScene>(State::Game);
     *
     *         _scenes.attach(*this);            // wire into the app's state machine
     *         _scenes.start (State::MainMenu);  // first scene, no history entry
     *     }
     *
     *     void onReady() override
     *     {
     *         // Navigation is just setState — SceneManager handles the rest
     *         _mainMenu.playButton().onClick([this]{ setState(State::Game); });
     *         _mainMenu.settingsButton().onClick([this]{ setState(State::Settings); });
     *         _settings.backButton().onClick([this]{ _scenes.back(); });
     *     }
     * };
     * @endcode
     *
     * ### Sub-routing inside a Component
     * @code
     * class Workspace : public ml::Component<WorkspaceManifest>
     * {
     *     using State  = WorkspaceManifest::State;   // { Chat, Quiz, Monitor }
     *     using Scenes = ml::SceneManager<State>;
     *
     *     Scenes     _scenes;   // this component's own sub-router
     *     ChatPanel  _chat;
     *     QuizPanel  _quiz;
     *
     * public:
     *     Workspace()
     *     {
     *         _scenes.bind(State::Chat, _chat);
     *         _scenes.bind(State::Quiz, _quiz);
     *         _scenes.attach(*this);            // attaches to THIS component
     *         _scenes.start (State::Chat);
     *     }
     * };
     * @endcode
     *
     * ### Lazy scenes
     * Lazy scenes are heap-allocated on first visit and freed when they are
     * navigated away from. This is useful for heavy scenes (lots of textures,
     * many components) that should not occupy memory while inactive.
     *
     * @code
     * _scenes.bindLazy<GameScene>(State::Game);          // no args
     * _scenes.bindLazy<GameScene>(State::Game, level);   // constructor args forwarded
     * @endcode
     *
     * ### History and back()
     * Every @c setState() call pushes the departing state onto a history stack.
     * @c back() pops it and calls @c setState() with that value. The initial
     * scene set via @c start() is NOT pushed — it acts as the history root.
     *
     * @tparam StateEnum  The @c enum @c class declared in the owner's manifest's
     *                    @c State typedef. Must match the enum used by the owner's
     *                    own state machine.
     *
     * @see ApplicationWith, Component, StateManager, CoreManager, Core
     */
    template<typename StateEnum>
    class SceneManager
    {
        // ── Internal scene entry ────────────────────────────────────────────

        /// @cond INTERNAL
        struct SceneEntry
        {
            Core*                    scene;    ///< Eager: raw pointer to bound scene.
            std::function<Core*()>   factory;  ///< Lazy: constructs a new instance.
            std::unique_ptr<Core>    owned;    ///< Lazy: owns the live instance.

            bool isLazy()   const { return static_cast<bool>(factory); }
            bool isActive() const { return scene != nullptr; }

            /// Returns the live Core pointer regardless of eager/lazy mode.
            Core* get() const { return scene; }
        };
        /// @endcond

        // ── Per-instance storage ────────────────────────────────────────────

        std::unordered_map<StateEnum, SceneEntry, EnumClassHash> _bindings;
        std::vector<StateEnum>        _history;
        StateEnum                     _current{};
        bool                          _started  = false;
        bool                          _attached = false;

        /// Called to add a scene to the owner. Wired via attach().
        std::function<void(Core&)>       _add;

        /// Called to remove a scene from the owner. Wired via attach().
        std::function<void(Core&)>       _remove;

        /// Calls setState() on the owner. Used by back().
        std::function<void(StateEnum)>   _setter;

        /// Silently syncs the owner's state machine current value without callbacks.
        std::function<void(StateEnum)>   _sync;

    public:

        // ── Lifetime ──────────────────────────────────────────────────────────

        SceneManager() = default;

        // Non-copyable, non-movable: attach() captures `this` into the owner's
        // callbacks, so the instance must keep a stable address while attached.
        SceneManager(const SceneManager&)            = delete;
        SceneManager& operator=(const SceneManager&) = delete;
        SceneManager(SceneManager&&)                 = delete;
        SceneManager& operator=(SceneManager&&)      = delete;

        // ── Bind ────────────────────────────────────────────────────────────

        /**
         * @brief Bind an existing scene instance to a state (eager).
         *
         * The scene is always in memory. @c SceneManager does not own it —
         * lifetime management is the caller's responsibility (typically a
         * member of the owning Application/Component).
         *
         * @param state  The state that activates this scene.
         * @param scene  Reference to a @c Core-derived scene object.
         */
        void bind(StateEnum state, Core& scene)
        {
            scene.setEnabled(false);
            _bindings[state] = SceneEntry{ &scene, nullptr, nullptr };
        }

        /**
         * @brief Bind a lazily-constructed scene type to a state.
         *
         * The scene is heap-allocated on first visit and destroyed when
         * navigated away from. Constructor arguments are forwarded at
         * construction time.
         *
         * @tparam SceneType  A @c Core-derived type to construct on demand.
         * @tparam Args       Constructor argument types (deduced).
         * @param  state      The state that activates this scene.
         * @param  args       Arguments forwarded to @c SceneType's constructor.
         *
         * @code
         * _scenes.bindLazy<GameScene>(State::Game);
         * _scenes.bindLazy<LevelScene>(State::Level, levelIndex);
         * @endcode
         */
        template<typename SceneType, typename... Args>
        void bindLazy(StateEnum state, Args&&... args)
        {
            static_assert(std::is_base_of_v<Core, SceneType>,
                "[SceneManager] bindLazy: SceneType must derive from ml::Core.");

            // Capture args by value so the factory is self-contained
            _bindings[state] = SceneEntry
            {
                nullptr,
                [capturedArgs = std::make_tuple(std::forward<Args>(args)...)]() mutable -> Core*
                {
                    return std::apply([](auto&&... a) -> Core*
                    {
                        return new SceneType(std::forward<decltype(a)>(a)...);
                    }, capturedArgs);
                },
                nullptr
            };
        }

        // ── Attach ──────────────────────────────────────────────────────────

        /**
         * @brief Wire @c SceneManager into the owner's state machine.
         *
         * Registers @c onStateEnter and @c onStateExit callbacks on @p owner
         * so that every future @c setState() call automatically triggers a
         * scene swap. Also stores a reference to @c setState() so that
         * @c back() can navigate without the caller holding an owner reference.
         *
         * Call this once after construction (e.g. in @c onInit() for an app, or
         * the constructor for a component), before @c start().
         *
         * @tparam Owner  Any class that exposes:
         *                - @c onStateEnter(std::function<void(StateEnum)>)
         *                - @c onStateExit(std::function<void(StateEnum)>)
         *                - @c setState(StateEnum)
         *                - @c syncState(StateEnum)
         *                - @c addComponent(Core&)
         *                - @c removeComponent(Core&)
         *
         * @param owner  The Application or Component instance to attach to.
         *
         * @warning @p owner and @c *this must both outlive the attachment.
         */
        template<typename Owner>
        void attach(Owner& owner)
        {
            _attached = true;

            // Store navigation primitives
            _setter = [&owner](StateEnum s) { owner.setState(s); };
            _sync   = [&owner](StateEnum s) { owner.syncState(s); };
            _add    = [&owner](Core& s)     { owner.addComponent(s); };
            _remove = [&owner](Core& s)     { owner.removeComponent(s); };

            // Hook into the owner's state machine — SceneManager reacts to transitions
            owner.onStateExit([this](StateEnum leaving)
            {
                deactivate(leaving);
            });

            owner.onStateEnter([this](StateEnum entering)
            {
                activate(entering);
            });
        }

        // ── Start ───────────────────────────────────────────────────────────

        /**
         * @brief Activate the initial scene without recording a history entry.
         *
         * Call after @c attach() and all @c bind() / @c bindLazy() calls.
         * The initial scene is treated as the history root — @c back() cannot
         * navigate past it.
         *
         * @param state  The state whose scene should be shown first.
         */
        void start(StateEnum state)
        {
            _current = state;
            _started = true;
            if (_sync) _sync(state);  // sync owner state machine without triggering callbacks
            activate(state);          // show first scene — no history push
        }

        // ── Navigation ──────────────────────────────────────────────────────

        /**
         * @brief Navigate to the previously visited state.
         *
         * Pops the history stack and calls @c setState() with the recovered
         * state. Does nothing if history is empty (i.e., already at the root).
         *
         * @note This calls @c setState() on the owner, which fires
         *       @c onStateExit and @c onStateEnter as normal. The scene swap
         *       happens automatically through those callbacks.
         */
        void back()
        {
            if (_history.empty()) return;

            StateEnum previous = _history.back();
            _history.pop_back();

            // Call setState without re-pushing to history
            // We set _current ahead of time so activate() knows not to push
            _current = previous;

            if (_setter)
                _setter(previous);
        }

        // ── Query ───────────────────────────────────────────────────────────

        /**
         * @brief Return the currently active state.
         * @return The state whose scene is currently shown.
         */
        StateEnum current() const { return _current; }

        /**
         * @brief Return @c true if @p state is currently the active scene.
         * @param state  State to check.
         */
        bool isActive(StateEnum state) const { return _started && _current == state; }

        /**
         * @brief Return @c true if a scene has been bound to @p state.
         * @param state  State to check.
         */
        bool has(StateEnum state) const { return _bindings.count(state) > 0; }

        /**
         * @brief Clear all bindings and history.
         *
         * Call before rebuilding the scene set. Does NOT deactivate the current
         * scene — call @c deactivate(current()) first if needed.
         */
        void clear()
        {
            _bindings.clear();
            _history.clear();
            _started  = false;
            _attached = false;
            _add    = nullptr;
            _remove = nullptr;
            _setter = nullptr;
            _sync   = nullptr;
        }

    private:

        // ── Internal transition helpers ─────────────────────────────────────

        /**
         * @brief Add the scene bound to @p state to the owner.
         *
         * For lazy scenes, constructs the instance on first visit.
         *
         * @param state  The state being entered.
         */
        void activate(StateEnum state)
        {
            auto it = _bindings.find(state);
            if (it == _bindings.end()) return;

            SceneEntry& entry = it->second;

            if (entry.isLazy())
            {
                // Construct on demand if not already alive
                if (!entry.owned)
                {
                    entry.owned.reset(entry.factory());
                    entry.scene = entry.owned.get();
                }
            }

            if (entry.scene && _add)
            {
                entry.scene->setEnabled(true);
                _add(*entry.scene);
            }
        }

        /**
         * @brief Remove the scene bound to @p state from the owner.
         *
         * For lazy scenes, frees the instance after removal.
         * Pushes @p state to history so @c back() can return to it.
         *
         * @param state  The state being exited.
         */
        void deactivate(StateEnum state)
        {
            auto it = _bindings.find(state);
            if (it == _bindings.end()) return;

            SceneEntry& entry = it->second;

            if (entry.scene && _remove)
            {
                _remove(*entry.scene);
                entry.scene->setEnabled(false);
            }

            // Push to history so back() can return
            _history.push_back(state);
            _current = state; // will be overwritten by activate() shortly

            if (entry.isLazy())
            {
                // Free the lazy instance — next visit will reconstruct
                entry.scene = nullptr;
                entry.owned.reset();
            }
        }
    };

} // namespace ml

#endif // MALENA_SCENEMANAGER_H
