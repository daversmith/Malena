// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

//
// Anchor.h — a single retained relational-placement constraint.
//
// An Anchor records "keep me <op> <reference> by <spacing>". The reference is a
// live Positionable (re-read each solve) or the window (ref == nullptr). Anchors
// are stored per-object by AnchorManager and re-solved on window resize, so a
// layout expressed with the Align verbs stays correct when the window changes.
//

#ifndef MALENA_ANCHOR_H
#define MALENA_ANCHOR_H

#pragma once

namespace ml
{
    class Positionable;

    /// Relational placement operation. X-ops touch only the x axis, Y-ops only y,
    /// and the Center/CenterOn ops the noted axes — so an x-op and a y-op compose.
    enum class AnchorOp
    {
        LeftOf,   ///< my right edge to ref's left edge  (x)
        RightOf,  ///< my left edge to ref's right edge  (x)
        Above,    ///< my bottom edge to ref's top edge  (y)
        Below,    ///< my top edge to ref's bottom edge  (y)
        CenterX,  ///< center within ref horizontally     (x)
        CenterY,  ///< center within ref vertically       (y)
        CenterOn  ///< center within ref both axes        (x + y)
    };

    /// The axis (or axes) an op writes — used to de-duplicate and reason about
    /// which stored anchors compose vs. override one another.
    inline bool anchorTouchesX(AnchorOp op)
    {
        return op == AnchorOp::LeftOf || op == AnchorOp::RightOf
            || op == AnchorOp::CenterX || op == AnchorOp::CenterOn;
    }
    inline bool anchorTouchesY(AnchorOp op)
    {
        return op == AnchorOp::Above || op == AnchorOp::Below
            || op == AnchorOp::CenterY || op == AnchorOp::CenterOn;
    }

    struct Anchor
    {
        AnchorOp            op;
        const Positionable* ref;      ///< live reference; nullptr == the window
        float               spacing;  ///< gap / offset in pixels
    };

} // namespace ml

#endif // MALENA_ANCHOR_H
