#ifndef CIRCLE_H
#define CIRCLE_H

#pragma once

#include <Malena/Graphics/Base/Graphic.h>
#include <Malena/Traits/Customizable.h>

namespace ml
{
    /**
     * @brief A framework-integrated circle shape.
      * @ingroup GraphicsPrimitives
     *
     * @c Circle wraps @c sf::CircleShape inside @c Graphic<>, giving it
     * the full framework trait set: @c Subscribable (click, hover, update),
     * @c Flaggable, @c Positionable, and @c Draggable.
     *
     * ### Usage
     * @code
     * ml::Circle dot;
     * dot.setRadius(25.f);
     * dot.setPosition({200.f, 200.f});
     * dot.setFillColor(sf::Color::Red);
     * dot.onClick([]{ std::cout << "dot clicked\n"; });
     * addComponent(dot);
     * @endcode
     *
     * ### With a manifest
     * @c CircleWith<Manifest> layers a manifest's flags and states onto
     * the circle:
     * @code
     * class MyDot : public ml::CircleWith<MyManifest> {};
     * @endcode
     *
     * @see Graphic, CircleButton
     */
    class Circle : public Graphic<sf::CircleShape>
    {
    public:
        using Graphic::Graphic;
    };

    /**
     * @brief @c Circle with an attached manifest.
     *
     * @tparam MANIFEST A @c Manifest subclass declaring @c Flag/@c State enums.
     *
     * @see Circle, Customizable
     */
    template<typename MANIFEST>
    class CircleWith : public Circle, public Customizable<MANIFEST> {};

} // namespace ml

#endif // CIRCLE_H
