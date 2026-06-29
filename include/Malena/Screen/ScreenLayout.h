// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#pragma once
#ifndef MALENA_SCREEN_LAYOUT_H
#define MALENA_SCREEN_LAYOUT_H

#include <Malena/Core/malena_export.h>
#include <Malena/Graphics/Base/Graphic.h>
#include <Malena/Screen/ScreenReceiver.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <string>

namespace ml {

/**
 * @brief A two-pane screen-share viewport: one stream, or two split.
 *
 * Holds two ScreenReceivers and arranges them inside its own rect according to a
 * @c Mode (Single / SideBySide / AboveBelow) and a split @c ratio (pane-0's
 * share, 0–1). Each pane points at its own RTSP source (switchable at runtime via
 * @c setSource) and can be frozen independently.
 *
 * One widget shared by every screen-share surface: the front-of-room monitor, the
 * admin's live preview, and a student-device viewer all instantiate a ScreenLayout
 * and are driven identically by the director's control messages. Display-only —
 * it has no interactive children; it just lays out and draws its panes each frame
 * (so it tracks size/position changes for free).
 */
class MALENA_API ScreenLayout : public Graphic<sf::RectangleShape>
{
public:
    enum class Mode { Single, SideBySide, AboveBelow };

    ScreenLayout();

    /** Composition mode. Single shows pane 0 only; the splits show both. */
    void setMode(Mode mode);
    Mode mode() const;

    /** Pane-0's share of the split (0–1); the rest goes to pane 1. Clamped. */
    void setRatio(float ratio01);
    float ratio() const;

    /** Aim a pane (0 or 1) at an RTSP source; restarts that pane if running. */
    void setSource(int pane, const std::string& rtspUrl);

    /** Freeze/unfreeze a pane (0 or 1) — holds its last frame. */
    void setFrozen(int pane, bool frozen);

    /** How both panes fit their frame into their rect (Stretch/Fit/Fill). */
    void setScaleMode(ScreenReceiverBase::ScaleMode mode);

    /** Start / stop both panes' pipelines (panes with no source stay idle). */
    void start();
    void stop();

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    void layoutPanes() const;   // const: invoked from draw() to track live size

    mutable ScreenReceiverBase _pane0;
    mutable ScreenReceiverBase _pane1;
    Mode  _mode    = Mode::Single;
    float _ratio   = 0.5f;
    bool  _running = false;
};

} // namespace ml

#endif // MALENA_SCREEN_LAYOUT_H
