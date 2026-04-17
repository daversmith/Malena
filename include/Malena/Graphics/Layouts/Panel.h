//
// Created by Dave Smith on 4/1/26.
//

#ifndef MALENA_PANEL_H
#define MALENA_PANEL_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/CoreManager.h>
#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <Malena/Utilities/HasSetSize.h>
#include <functional>
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

        // Resize lambdas for fill-enabled children (empty function = no setSize)
        std::unordered_map<Core*, std::function<void(sf::Vector2f)>> _fillChildren;

    public:
        Panel();

        /**
         * @brief Add a child component to this panel.
         *
         * The child's current absolute position is recorded relative to the
         * panel's current position. Subsequent @c setPosition() calls will
         * keep the child at the same relative offset.
         *
         * @param child  The component to add.
         * @param fill   If @c true (default) and the child has @c setSize,
         *               it will be resized to match this panel whenever the
         *               panel resizes. Pass @c false to manage size manually.
         */
        template<typename T>
        void addComponent(T& child, bool fill = false)
        {
            static_assert(std::is_base_of_v<Core, T>,
                "Panel::addComponent requires a Core-derived type");

            if (fill)
            {
                if constexpr (detail::has_setSize<T>::value)
                {
                    // Fill child: snap to panel origin, fill its size
                    child.setPosition(getPosition());
                    child.setSize(getSize());
                    _relativePositions[&child] = {0.f, 0.f};
                    _fillChildren[&child] = [&child](sf::Vector2f sz){ child.setSize(sz); };
                }
                else
                {
                    _relativePositions[&child] = child.getPosition() - getPosition();
                }
            }
            else
            {
                _relativePositions[&child] = child.getPosition() - getPosition();
            }

            CoreManager<Core>::addComponent(child);
        }

        /**
         * @brief Add a child by reference — drawn by the panel but position
         *        and size are managed entirely by the caller.
         *
         * Unlike @c addComponent(), the panel never calls @c setPosition or
         * @c setSize on the child. Use this when the component is already
         * positioned absolutely in world space and you only need the panel
         * to include it in its draw pass.
         *
         * @code
         * // myButton is positioned elsewhere — panel just draws it
         * panel.addRef(myButton);
         * @endcode
         */
        void addRef(Core& child);

        /**
         * @brief Remove a child component from this panel.
         */
        bool removeComponent(Core& child);

        /**
         * @brief Resize the panel and propagate to fill-enabled children.
         */
        void setSize(const sf::Vector2f& size);

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
