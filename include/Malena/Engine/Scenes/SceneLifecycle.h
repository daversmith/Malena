//
// SceneLifecycle.h
//

#ifndef SCENELIFECYCLE_H
#define SCENELIFECYCLE_H

#pragma once

#include <Malena/Core/malena_export.h>

namespace ml
{
    /**
     * @brief Trait that adds scene transition lifecycle hooks to any class.
     * @ingroup Traits
     *
     * @c SceneLifecycle provides four virtual hooks that fire on scene
     * transitions managed by @c SceneManager. All have empty default
     * implementations — override only what you need.
     *
     * | Hook              | When it fires                                         |
     * |-------------------|-------------------------------------------------------|
     * | @c onSceneEnter() | Every time this scene becomes the active scene        |
     * | @c onSceneExit()  | Every time this scene is deactivated                  |
     * | @c onScenePause() | This scene is pushed to the background by another     |
     * | @c onSceneResume()| This scene is returned to the foreground              |
     *
     * The key distinction from @c Lifecycle is that these hooks fire on every
     * transition, not just once at startup. A cached scene that is entered,
     * exited, and re-entered will have @c onSceneEnter() called twice but
     * @c Lifecycle::onInit() called only once.
     *
     * ### Hook firing order
     * @code
     * // First activation
     * onSceneEnter()
     *
     * // Another scene is pushed on top
     * onScenePause()
     *
     * // Returned to foreground
     * onSceneResume()
     *
     * // Scene is deactivated and removed
     * onSceneExit()
     * @endcode
     *
     * ### Usage
     * @code
     * class GameScene : public ml::Scene, public ml::SceneLifecycle
     * {
     * protected:
     *     void onSceneEnter() override
     *     {
     *         // Resume music, reset timers, show entry animation
     *     }
     *
     *     void onSceneExit() override
     *     {
     *         // Pause music, persist state
     *     }
     *
     *     void onScenePause() override
     *     {
     *         // Freeze physics, dim the scene
     *     }
     *
     *     void onSceneResume() override
     *     {
     *         // Unfreeze physics, restore brightness
     *     }
     * };
     * @endcode
     *
     * Combine with @c Lifecycle when the scene also needs one-time
     * initialization hooks:
     * @code
     * class GameScene : public ml::Scene,
     *                   public ml::Lifecycle,
     *                   public ml::SceneLifecycle
     * { ... };
     * @endcode
     *
     * @see Lifecycle, Updatable, SceneManager
     */
    class MALENA_API SceneLifecycle
    {
    public:
        /**
         * @brief Called every time this scene becomes the active scene.
         *
         * Override to resume audio, reset timers, trigger entry animations,
         * or perform any setup that should repeat on each activation.
         */
        virtual void onSceneEnter() {}

        /**
         * @brief Called every time this scene is deactivated.
         *
         * Override to pause audio, persist state, or clean up resources
         * that should not run while the scene is inactive.
         */
        virtual void onSceneExit() {}

        /**
         * @brief Called when this scene is pushed to the background by another scene.
         *
         * The scene remains alive and cached but is no longer the active scene.
         * Override to freeze simulation, suppress input handling, or dim visuals.
         */
        virtual void onScenePause() {}

        /**
         * @brief Called when this scene is returned to the foreground.
         *
         * Paired with @c onScenePause(). Override to restore state that was
         * suppressed when the scene was backgrounded.
         */
        virtual void onSceneResume() {}

        virtual ~SceneLifecycle() = default;
    };

} // namespace ml

#endif // SCENELIFECYCLE_H
