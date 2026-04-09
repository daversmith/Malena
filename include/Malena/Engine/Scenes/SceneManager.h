// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// SceneManager.h
//

#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

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
     * @brief State-driven scene router for Malena applications.
     * @ingroup EngineApp
     *
     * @c SceneManager<StateEnum> binds scenes (any @c Core-derived object or
     * lazily-constructed type) to application state enum values. When the app
     * calls @c setState(), @c SceneManager intercepts the transition via
     * @c onStateEnter / @c onStateExit and performs the scene swap automatically —
     * removing the outgoing scene from @c CoreManager and adding the incoming one.
     *
     * ### Design contract
     * - @c setState() on the application is the **only** navigation primitive.
     *   @c SceneManager reacts to it; it does not replace it.
     * - @c back() is the one exception — it calls @c setState() with the
     *   previously visited state, keeping the state machine as the single
     *   source of truth.
     * - Scenes bound by reference (@c bind) are always in memory. Scenes bound
     *   lazily (@c bindLazy) are constructed on first visit and destroyed on
     *   @c back() or when another scene replaces them.
     *
     * ### Typical setup
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
     *     using Scenes = ml::SceneManager<AppManifest::State>;
     *
     *     MainMenuScene _mainMenu;   // eager — always in memory
     *     SettingsScene _settings;   // eager
     *     // GameScene is lazy — only allocated when visited
     *
     * public:
     *     MyApp() : ml::ApplicationWith<AppManifest>(1280, 720, 32, "My Game") {}
     *
     *     void onInit() override
     *     {
     *         Scenes::bind    (State::MainMenu,  _mainMenu);
     *         Scenes::bind    (State::Settings,  _settings);
     *         Scenes::bindLazy<GameScene>(State::Game);
     *
     *         Scenes::attach(*this);            // wire into the app's state machine
     *         Scenes::start (State::MainMenu);  // first scene, no history entry
     *     }
     *
     *     void onReady() override
     *     {
     *         // Navigation is just setState — SceneManager handles the rest
     *         _mainMenu.playButton().onClick([this]{
     *             setState(State::Game);
     *         });
     *         _mainMenu.settingsButton().onClick([this]{
     *             setState(State::Settings);
     *         });
     *         _settings.backButton().onClick([]{
     *             Scenes::back();    // returns to MainMenu, calls setState internally
     *         });
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
     * Scenes::bindLazy<GameScene>(State::Game);          // no args
     * Scenes::bindLazy<GameScene>(State::Game, level);   // constructor args forwarded
     * @endcode
     *
     * ### History and back()
     * Every @c setState() call pushes the departing state onto a history stack.
     * @c back() pops it and calls @c setState() with that value. The initial
     * scene set via @c start() is NOT pushed — it acts as the history root.
     *
     * @tparam StateEnum  The @c enum @c class declared in your app manifest's
     *                    @c State typedef. Must match the enum used by the app's
     *                    own @c StateManager.
     *
     * @see ApplicationWith, StateManager, CoreManager, Core
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

        // ── Static storage ──────────────────────────────────────────────────

        inline static std::unordered_map<StateEnum, SceneEntry, EnumClassHash> _bindings;
        inline static std::vector<StateEnum>        _history;
        inline static StateEnum                     _current{};
        inline static bool                          _started  = false;
        inline static bool                          _attached = false;

        /// Called to add a scene to the CoreManager. Wired via attach().
        inline static std::function<void(Core&)>       _add;

        /// Called to remove a scene from the CoreManager. Wired via attach().
        inline static std::function<void(Core&)>       _remove;

        /// Calls setState() on the app. Used by back().
        inline static std::function<void(StateEnum)>   _setter;

    public:

        // ── Bind ────────────────────────────────────────────────────────────

        /**
         * @brief Bind an existing scene instance to a state (eager).
         *
         * The scene is always in memory. @c SceneManager does not own it —
         * lifetime management is the caller's responsibility (typically a
         * member of @c Application).
         *
         * @param state  The application state that activates this scene.
         * @param scene  Reference to a @c Core-derived scene object.
         */
        static void bind(StateEnum state, Core& scene)
        {
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
         * @param  state      The application state that activates this scene.
         * @param  args       Arguments forwarded to @c SceneType's constructor.
         *
         * @code
         * Scenes::bindLazy<GameScene>(State::Game);
         * Scenes::bindLazy<LevelScene>(State::Level, levelIndex);
         * @endcode
         */
        template<typename SceneType, typename... Args>
        static void bindLazy(StateEnum state, Args&&... args)
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
         * @brief Wire @c SceneManager into the application's state machine.
         *
         * Registers @c onStateEnter and @c onStateExit callbacks on @p app
         * so that every future @c setState() call automatically triggers a
         * scene swap. Also stores a reference to @c setState() so that
         * @c back() can navigate without the caller holding an app reference.
         *
         * Call this once in @c onInit(), before @c start().
         *
         * @tparam App  Any class that exposes:
         *              - @c onStateEnter(std::function<void(StateEnum)>)
         *              - @c onStateExit(std::function<void(StateEnum)>)
         *              - @c setState(StateEnum)
         *              - @c addComponent(Core&)
         *              - @c removeComponent(Core&)  [or via CoreManager]
         *
         * @param app  The application instance to attach to.
         */
        template<typename App>
        static void attach(App& app)
        {
            _attached = true;

            // Store navigation primitives
            _setter = [&app](StateEnum s) { app.setState(s); };
            _add    = [&app](Core& s)     { app.addComponent(s); };

            // removeComponent lives on CoreManager — call through app if available,
            // otherwise fall back to CoreManager<Core> directly
            _remove = [&app](Core& s)
            {
                if constexpr (requires { app.removeComponent(s); })
                    app.removeComponent(s);
                else
                    CoreManager<Core>::removeComponent(s);
            };

            // Hook into the app's state machine — SceneManager reacts to transitions
            app.onStateExit([](StateEnum leaving)
            {
                SceneManager::deactivate(leaving);
            });

            app.onStateEnter([](StateEnum entering)
            {
                SceneManager::activate(entering);
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
        static void start(StateEnum state)
        {
            _current = state;
            _started = true;
            activate(state);  // show first scene — no history push
        }

        // ── Navigation ──────────────────────────────────────────────────────

        /**
         * @brief Navigate to the previously visited state.
         *
         * Pops the history stack and calls @c setState() with the recovered
         * state. Does nothing if history is empty (i.e., already at the root).
         *
         * @note This calls @c setState() on the application, which fires
         *       @c onStateExit and @c onStateEnter as normal. The scene swap
         *       happens automatically through those callbacks.
         */
        static void back()
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
        static StateEnum current() { return _current; }

        /**
         * @brief Return @c true if @p state is currently the active scene.
         * @param state  State to check.
         */
        static bool isActive(StateEnum state) { return _started && _current == state; }

        /**
         * @brief Return @c true if a scene has been bound to @p state.
         * @param state  State to check.
         */
        static bool has(StateEnum state) { return _bindings.count(state) > 0; }

        /**
         * @brief Clear all bindings and history.
         *
         * Call before rebuilding the scene set (e.g., after @c Application::reset()).
         * Does NOT deactivate the current scene — call @c deactivate(current()) first
         * if needed.
         */
        static void clear()
        {
            _bindings.clear();
            _history.clear();
            _started  = false;
            _attached = false;
            _add    = nullptr;
            _remove = nullptr;
            _setter = nullptr;
        }

    private:

        // ── Internal transition helpers ─────────────────────────────────────

        /**
         * @brief Add the scene bound to @p state to the CoreManager.
         *
         * For lazy scenes, constructs the instance on first visit.
         * Pushes the previously active state to history unless this is
         * the very first activation (@c start()).
         *
         * @param state  The state being entered.
         */
        static void activate(StateEnum state)
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
                _add(*entry.scene);
        }

        /**
         * @brief Remove the scene bound to @p state from the CoreManager.
         *
         * For lazy scenes, frees the instance after removal.
         * Pushes @p state to history so @c back() can return to it.
         *
         * @param state  The state being exited.
         */
        static void deactivate(StateEnum state)
        {
            auto it = _bindings.find(state);
            if (it == _bindings.end()) return;

            SceneEntry& entry = it->second;

            if (entry.scene && _remove)
                _remove(*entry.scene);

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

#endif // SCENEMANAGER_H
