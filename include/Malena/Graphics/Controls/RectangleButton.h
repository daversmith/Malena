#ifndef RECTANGLEBUTTON_H
#define RECTANGLEBUTTON_H

#pragma once

#include <Malena/Graphics/Base/Button.h>
#include <Malena/Graphics/Primitives/Rectangle.h>

namespace ml
{
    /**
     * @brief A rectangular button with a centered text label.
      * @ingroup GraphicsControls
     *
     * @c RectangleButton is @c Button<Rectangle, sf::Vector2f> — a
     * @c Rectangle background shape with an @c sf::Text label rendered on
     * top. It inherits the full @c Rectangle feature set (rounded corners,
     * fill/outline color, @c Flaggable, @c Draggable) plus the @c Button
     * text API (setString, setTextColor, setCharacterSize, etc.).
     *
     * The size parameter is @c sf::Vector2f (width × height).
     *
     * ### Usage
     * @code
     * ml::RectangleButton btn;
     * btn.setSize({180.f, 50.f});
     * btn.setPosition({100.f, 200.f});
     * btn.setString("Start");
     * btn.setFillColor(sf::Color(60, 120, 200));
     * btn.setTextColor(sf::Color::White);
     * btn.onClick([]{ startGame(); });
     * addComponent(btn);
     * @endcode
     *
     * ### With a manifest
     * @c RectangleButtonWith<Manifest> adds manifest flag/state support:
     * @code
     * class MenuButton : public ml::RectangleButtonWith<MenuManifest> {};
     * @endcode
     *
     * @see Button, Rectangle, CircleButton, ConvexButton
     */
    class RectangleButton : public Button<Rectangle, sf::Vector2f>
    {
    public:
        using Button::Button;
    };

    /**
     * @brief @c RectangleButton with an attached manifest.
     *
     * @tparam MANIFEST A @c Manifest subclass declaring @c Flag/@c State enums.
     *
     * @see RectangleButton, Customizable
     */
    template<typename MANIFEST>
    class RectangleButtonWith : public RectangleButton, public Customizable<MANIFEST> {};

} // namespace ml

#endif // RECTANGLEBUTTON_H
