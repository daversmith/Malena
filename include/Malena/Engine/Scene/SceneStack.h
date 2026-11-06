// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

//
// SceneStack.h
//

#ifndef MALENA_SCENESTACK_H
#define MALENA_SCENESTACK_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Component.h>
#include <cstddef>
#include <deque>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace ml
{
    /**
     * @brief A stack of layered scenes where only the top scene is live.
     * @ingroup EngineApp
     *
     * @c SceneStack holds scenes (any @c Core-derived object) bottom to top.
     * The top scene is enabled; every scene under it is disabled, so its
     * clicks, hovers, key presses and updates stop until it is uncovered.
     * Because enabled-state cascades to children, this also covers every
     * control a scene added with @c addComponent: a covered menu's button
     * cannot be clicked, without the button checking anything itself.
     *
     * Use it for screens that stack on top of each other: a pause screen over
     * a game, a dialog over a menu, a game-over screen replacing the game.
     * For switching between fixed top-level places driven by a state enum,
     * use @c SceneManager. The two compose: a @c SceneStack can be one of a
     * @c SceneManager's scenes, and a scene can own a @c SceneStack of its own.
     *
     * | Call                   | Effect                                                   |
     * |------------------------|----------------------------------------------------------|
     * | @c push<T>(args...)    | Construct a @c T and put it on top                       |
     * | @c pushOverlay<T>(...) | Same, but the scene underneath stays drawn (frozen)      |
     * | @c pop(n)              | Remove and destroy the top @c n scenes                   |
     * | @c replace<T>(...)     | Destroy the top scene and put a @c T in its place        |
     * | @c clear()             | Destroy every scene                                      |
     *
     * ### Changes are deferred during events
     * A scene usually changes the stack from its own handler, as in a button
     * that pops its own screen. Destroying the scene right then would free the
     * object whose handler is still running. So a change requested while
     * Malena is dispatching an event is saved and applied on the stack's next
     * update, after the handler has returned. Changes requested outside event
     * dispatch, such as in @c onReady(), apply immediately. Saved changes are
     * applied in the order they were requested.
     *
     * ### Ownership and drawing
     * The stack owns its scenes and destroys them when they are popped.
     * Scenes are children of the stack, so the stack's default draw paints
     * them bottom to top. A scene that is covered by an opaque scene is
     * hidden; one under an overlay stays visible but disabled.
     *
     * ### Lifecycle hooks
     * Scenes that also inherit @c SceneLifecycle are told about transitions:
     *
     * | Hook               | When                                        |
     * |--------------------|---------------------------------------------|
     * | @c onSceneEnter()  | Pushed onto the stack                       |
     * | @c onScenePause()  | Another scene was pushed on top of it       |
     * | @c onSceneResume() | The scene above it was popped               |
     * | @c onSceneExit()   | Popped, replaced or cleared (before delete) |
     *
     * ### Usage
     * @code
     * class GameApp : public ml::Application
     * {
     *     ml::SceneStack _screens;
     * public:
     *     GameApp() : ml::Application(720, 420, 32, "Game") {}
     *     void onInit() override {}
     *     void onReady() override
     *     {
     *         _screens.push<MainMenu>(_screens);
     *         addComponent(_screens);
     *     }
     * };
     *
     * MainMenu::MainMenu(ml::SceneStack& screens) : _screens(screens)
     * {
     *     addComponent(_play);                    // a child, so the stack can disable it
     *     _play.onClick([this] { _screens.push<Game>(_screens); });
     * }
     *
     * void Game::onPauseKey() { _screens.pushOverlay<PauseMenu>(_screens); }
     * void PauseMenu::onResume() { _screens.pop(); }
     * @endcode
     *
     * A scene that didn't keep a reference can find its stack with
     * @c SceneStack::of(*this).
     *
     * @note A scene's own controls must be its children (@c addComponent)
     *       for the stack to disable them. A control that is only a member,
     *       and subscribes to events itself, keeps receiving them while the
     *       scene is covered.
     * @note The stack enables the top scene and disables the rest after every
     *       change. Calling @c setEnabled on a scene yourself is overridden on
     *       the next change.
     *
     * @see SceneManager, SceneLifecycle
     */
    class MALENA_API SceneStack : public Component<>
    {
    public:
        /// How a scene treats the scenes underneath it.
        enum class Mode
        {
            Opaque,    ///< Scenes underneath are hidden.
            Overlay    ///< The scene underneath stays drawn (but disabled).
        };

        SceneStack();
        ~SceneStack() override;

        // Scenes and saved changes hold pointers back to this stack.
        SceneStack(const SceneStack&)            = delete;
        SceneStack& operator=(const SceneStack&) = delete;
        SceneStack(SceneStack&&)                 = delete;
        SceneStack& operator=(SceneStack&&)      = delete;

        // ── Changes ─────────────────────────────────────────────────────────

        /**
         * @brief Put @p scene on top. The stack takes ownership.
         * @param scene  The scene to push. Ignored if null.
         * @param mode   Whether the scene underneath stays drawn.
         */
        void push(std::unique_ptr<Core> scene, Mode mode = Mode::Opaque);

        /**
         * @brief Construct a @c T from @p args and put it on top.
         * @return The new scene. It exists immediately, even when the push
         *         itself is deferred, so it can be configured right away.
         */
        template<typename T, typename... Args>
        T& push(Args&&... args)
        {
            return pushAs<T>(Mode::Opaque, std::forward<Args>(args)...);
        }

        /// Like @c push<T>, but the scene underneath stays drawn while covered.
        template<typename T, typename... Args>
        T& pushOverlay(Args&&... args)
        {
            return pushAs<T>(Mode::Overlay, std::forward<Args>(args)...);
        }

        /// Remove and destroy the top @p count scenes. Popping more scenes
        /// than the stack holds empties it.
        void pop(std::size_t count = 1);

        /**
         * @brief Destroy the top scene and put @p scene in its place.
         *
         * Unlike @c pop() followed by @c push(), the scene underneath is not
         * resumed and paused in between. On an empty stack this is a push.
         */
        void replace(std::unique_ptr<Core> scene, Mode mode = Mode::Opaque);

        /// Construct a @c T from @p args and put it in place of the top scene.
        template<typename T, typename... Args>
        T& replace(Args&&... args)
        {
            static_assert(std::is_base_of_v<Core, T>, "SceneStack scenes must derive from ml::Core");
            auto scene = std::make_unique<T>(std::forward<Args>(args)...);
            T& ref = *scene;
            replace(std::move(scene), Mode::Opaque);
            return ref;
        }

        /// Destroy every scene, including ones pushed earlier that are still
        /// waiting. Pushes requested after @c clear() are kept.
        void clear();

        // ── Query ───────────────────────────────────────────────────────────

        /// The live top scene, or @c nullptr when the stack is empty.
        /// Saved changes are not reflected until they are applied.
        Core* top() const;

        /// Number of scenes on the stack, not counting saved changes.
        std::size_t size() const;

        /// @c true when the stack holds no scenes.
        bool empty() const;

        /// @c true while changes are saved and waiting to be applied.
        bool hasPendingChanges() const;

        /**
         * @brief The stack that holds @p scene, or @c nullptr.
         *
         * Finds scenes on a stack and scenes whose push is still waiting.
         * Lets a scene change its own stack without keeping a reference:
         * @code
         * ml::SceneStack::of(*this)->pop();
         * @endcode
         */
        static SceneStack* of(const Core& scene);

    private:
        /// @cond INTERNAL
        struct Layer
        {
            std::unique_ptr<Core> scene;
            Mode                  mode;
            bool                  enabled;   ///< Last state refresh() set.
            bool                  visible;   ///< Last state refresh() set.
        };

        struct Change
        {
            enum class Type { Push, Pop, Replace, Clear } type;
            std::unique_ptr<Core> scene;   ///< Push and Replace only.
            Mode                  mode = Mode::Opaque;
        };

        template<typename T, typename... Args>
        T& pushAs(Mode mode, Args&&... args)
        {
            static_assert(std::is_base_of_v<Core, T>, "SceneStack scenes must derive from ml::Core");
            auto scene = std::make_unique<T>(std::forward<Args>(args)...);
            T& ref = *scene;
            push(std::move(scene), mode);
            return ref;
        }

        void request(Change change);
        void applyPending();
        void apply(Change& change);
        void pushLayer(std::unique_ptr<Core> scene, Mode mode, bool pauseBelow);
        void popLayer(bool resumeBelow);
        void refresh();
        bool holds(const Core& scene) const;

        std::vector<Layer>  _layers;     ///< Bottom to top.
        std::deque<Change>  _pending;    ///< Saved changes, oldest first.
        bool                _applying  = false;
        int                 _nextLayer = 0;   ///< Child layer key: later pushes draw on top.

        inline static std::vector<SceneStack*> _live;   ///< For of().
        /// @endcond
    };

} // namespace ml

#endif // MALENA_SCENESTACK_H
