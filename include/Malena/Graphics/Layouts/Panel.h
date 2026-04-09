//
// Created by Dave Smith on 4/1/26.
//

#ifndef PANEL_H
#define PANEL_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/CoreManager.h>
#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <unordered_map>

namespace ml {

    class MALENA_API PanelManifest : public Manifest {};

    /**
     * @brief A layout container that groups, positions, and draws a set of child components.
     *
     * Children are added via @c addComponent(). The Panel is responsible for:
     * - Drawing all children in its @c draw() pass.
     * - Propagating position changes to children (delta-shift) so that layout
     *   containers like @c SplitPanel can move an entire pane by repositioning
     *   its Panel.
     * - Automatically matching the active theme's @c surface color as its fill.
     *
     * Children receive framework events (update, click, hover) automatically
     * via the Fireable system — no separate @c Application::addComponent() call
     * is needed.
     *
     * ### Usage as SplitPanel pane content
     * @code
     * auto& pane = split.addPane(std::make_unique<ml::Panel>(), 300.f);
     * // Panel is now positioned — add children at their absolute positions
     * pane.addComponent(myInput);
     * pane.addComponent(myList);
     * @endcode
     */
    class MALENA_API Panel : public CoreManager<Core>, public RectangleWith<PanelManifest>, public Themeable
    {
        // Maps each child pointer to its position relative to this panel's origin
        std::unordered_map<Core*, sf::Vector2f> _relativePositions;

    public:
        Panel();

        /**
         * @brief Add a child component to this panel.
         *
         * The child's current absolute position is recorded relative to the
         * panel's current position. Subsequent @c setPosition() calls will
         * keep the child at the same relative offset.
         */
        void addComponent(Core& child);

        /**
         * @brief Remove a child component from this panel.
         */
        bool removeComponent(Core& child);

        /**
         * @brief Move the panel and shift all children by the same delta.
         */
        void setPosition(const sf::Vector2f& position) override;

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    protected:
        void onThemeApplied(const Theme& theme) override;
    };

} // ml

#endif //PANEL_H
