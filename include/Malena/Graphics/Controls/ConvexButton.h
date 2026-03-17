#ifndef CONVEXBUTTON_H
#define CONVEXBUTTON_H

#pragma once

#include <Malena/Graphics/Button.h>
#include <Malena/Graphics/Convex.h>
#include <Malena/Traits/Customizable.h>

namespace ml
{
    /**
     * @brief A convex polygon button with a centered text label.
     *
     * @c ConvexButton is @c Button<Convex, std::size_t> — a @c Convex
     * polygon background with an @c sf::Text label rendered on top.
     * The size parameter is a @c std::size_t point count, passed to
     * @c sf::ConvexShape::setPointCount.
     *
     * ### Usage
     * @code
     * ml::ConvexButton btn(ml::FontManager<>::getDefault(), 6, "Hex");
     * // Configure the six polygon points via the inherited setPoint() API
     * btn.setPoint(0, {60.f,  0.f});
     * btn.setPoint(1, {120.f, 30.f});
     * // ...
     * btn.setFillColor(sf::Color(80, 160, 80));
     * btn.onClick([]{ activate(); });
     * addComponent(btn);
     * @endcode
     *
     * ### With a manifest
     * @code
     * class HexButton : public ml::ConvexButtonWith<MyManifest> {};
     * @endcode
     *
     * @see Button, Convex, RectangleButton, CircleButton
     */
    class ConvexButton : public Button<Convex, std::size_t>
    {
        using Button::Button;
    };

    /**
     * @brief @c ConvexButton with an attached manifest.
     *
     * @tparam MANIFEST A @c Manifest subclass declaring @c Flag/@c State enums.
     *
     * @see ConvexButton, Customizable
     */
    template<typename MANIFEST>
    class ConvexButtonWith : public virtual Customizable<ConvexButton, MANIFEST> {};

} // namespace ml

#endif // CONVEXBUTTON_H
