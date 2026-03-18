#ifndef CIRCLEBUTTON_H
#define CIRCLEBUTTON_H

#pragma once

#include <Malena/Graphics/Base/Button.h>
#include <Malena/Graphics/Primitives/Circle.h>

namespace ml
{
    /**
     * @brief A circular button with a centered text label.
      * @ingroup GraphicsControls
     *
     * @c CircleButton is @c Button<Circle, float> — a @c Circle background
     * shape with an @c sf::Text label rendered on top. The size parameter
     * is a @c float radius.
     *
     * ### Usage
     * @code
     * ml::CircleButton btn(ml::FontManager<>::getDefault(), 40.f, "OK");
     * btn.setPosition({200.f, 200.f});
     * btn.setFillColor(sf::Color(200, 80, 80));
     * btn.setTextColor(sf::Color::White);
     * btn.onClick([]{ confirm(); });
     * addComponent(btn);
     * @endcode
     *
     * ### With a manifest
     * @code
     * class IconButton : public ml::CircleButtonWith<MyManifest> {};
     * @endcode
     *
     * @see Button, Circle, RectangleButton, ConvexButton
     */
    class CircleButton : public Button<Circle, float>
    {
        using Button::Button;
    };

    /**
     * @brief @c CircleButton with an attached manifest.
     *
     * @tparam MANIFEST A @c Manifest subclass declaring @c Flag/@c State enums.
     *
     * @see CircleButton, Customizable
     */
    template<typename MANIFEST>
    class CircleButtonWith : public CircleButton, public virtual Customizable<MANIFEST> {};

} // namespace ml

#endif // CIRCLEBUTTON_H
