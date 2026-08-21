// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

//
// AnchorManager.h — owns retained anchors and re-solves them.
//
// Anchors live here (external registry) rather than on Positionable, so the trait
// keeps no data member and stays copyable. AppManager calls solveAll() on window
// resize; a solve re-reads each reference's current bounds and re-applies the
// stored Align ops. An object moved externally between solves (a direct
// setPosition, or an animation) is detected and detached — manual control wins.
//

#ifndef MALENA_ANCHORMANAGER_H
#define MALENA_ANCHORMANAGER_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Layout/Anchor.h>
#include <cstddef>
#include <unordered_map>

namespace ml
{
    class Positionable;

    class MALENA_API AnchorManager
    {
    public:
        /// Record/replace a retained anchor for @p owner. @p ref == nullptr anchors
        /// to the window. An op replaces any existing anchor with the same op, so
        /// repeated fill() calls don't accumulate.
        static void set(Positionable* owner, AnchorOp op, const Positionable* ref, float spacing);

        /// Snapshot @p owner's current position as the baseline for external-move
        /// detection. Called right after a verb applies its op immediately.
        static void recordBaseline(Positionable* owner);

        /// Drop all of @p owner's anchors (the unanchored() opt-out).
        static void clear(Positionable* owner);

        /// True if @p owner currently has any retained anchor.
        static bool hasAnchors(const Positionable* owner);

        /// Remove @p owner's own anchors AND scrub any anchor (in any object) that
        /// references it. Called from ~Positionable so a destroyed object is never
        /// solved against or dangled.
        static void release(const Positionable* owner);

        /// Re-solve every anchored object (called on window resize). Detaches
        /// externally-moved objects first, then iterates to a fixpoint.
        static void solveAll();

        /// Number of objects with at least one anchor (test/introspection helper).
        static std::size_t anchoredCount();

    private:
        struct Entry;
        static std::unordered_map<Positionable*, Entry>& registry();
        static void solveOwner(Positionable* owner, Entry& e);
    };

} // namespace ml

#endif // MALENA_ANCHORMANAGER_H
