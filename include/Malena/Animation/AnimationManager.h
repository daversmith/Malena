// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

//
// AnimationManager.h — the single per-frame driver for all active animations.
//
// AppManager::run() calls advance(dt) once per frame. Each animation registers a
// type-erased step tagged with an owner pointer; when the owner is destroyed the
// framework cancels its steps so no dangling setter is ever called.
//

#ifndef MALENA_ANIMATIONMANAGER_H
#define MALENA_ANIMATIONMANAGER_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <cstddef>
#include <functional>
#include <vector>

namespace ml
{
    class MALENA_API AnimationManager
    {
    public:
        /// A per-frame step. Receives dt (seconds); returns true when finished.
        using Step = std::function<bool(float)>;

        /// Register an animation step owned by @p owner (used for cancellation).
        static void add(const void* owner, Step step);

        /// Advance every active animation by @p dt seconds (called once per frame).
        static void advance(float dt);

        /// Remove every animation belonging to @p owner (called on owner destruction).
        static void cancel(const void* owner);

        /// True if @p owner has any live animation.
        static bool hasActive(const void* owner);

        /// Total number of live animations (test/introspection helper).
        static std::size_t activeCount();

    private:
        struct Track
        {
            const void* owner = nullptr;
            Step        step;
            bool        dead  = false;
        };

        static std::vector<Track>& tracks();
        static std::vector<Track>& pending();
        static bool&               advancing();
    };

} // namespace ml

#endif // MALENA_ANIMATIONMANAGER_H
