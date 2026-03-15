#ifndef RECTANGLE_H
#define RECTANGLE_H

#pragma once

#include <Malena/Graphics/Base/Graphic.h>
#include <Malena/Traits/Customizable.h>
#include <Malena/Graphics/Primitives/RoundedRectangle.h>

namespace ml
{
    /**
     * @brief A framework-integrated rectangle with optional rounded corners.
     *
     * @c Rectangle wraps @c ml::RoundedRectangle inside @c Graphic<>, giving
     * it the full framework trait set: @c Subscribable (click, hover, update),
     * @c Flaggable, @c Positionable, and @c Draggable. It is one of the most
     * commonly used components for building UI layouts, panels, and buttons.
     *
     * Setting a corner radius of @c 0 (the default) produces a standard
     * sharp-cornered rectangle. Any positive radius produces rounded corners.
     *
     * ### Usage
     * @code
     * ml::Rectangle panel;
     * panel.setSize({300.f, 150.f});
     * panel.setPosition({50.f, 50.f});
     * panel.setFillColor(sf::Color(40, 40, 40));
     * panel.onClick([]{ std::cout << "panel clicked\n"; });
     * addComponent(panel);
     * @endcode
     *
     * ### With a manifest
     * @c RectangleWith<Manifest> layers a manifest's flags and states
     * onto the rectangle without changing its visual behavior:
     * @code
     * class MyWidget : public ml::RectangleWith<MyManifest> {};
     * @endcode
     *
     * @see RoundedRectangle, Graphic, RectangleButton
     */
    class Rectangle : public Graphic<ml::RoundedRectangle>
    {
    public:
        using Graphic::Graphic;
    };

    /**
     * @brief @c Rectangle with an attached manifest.
     *
     * Inherits @c Rectangle and @c Customizable<MANIFEST>, wiring the
     * manifest's @c Flag and @c State enums into the component's flag
     * and state stores.
     *
     * @tparam MANIFEST A @c Manifest subclass declaring @c Flag/@c State enums.
     *
     * @see Rectangle, Customizable
     */
    template<typename MANIFEST>
    class RectangleWith : public Rectangle, public Customizable<MANIFEST> {};

} // namespace ml

#endif // RECTANGLE_H
