// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_CIRCLEBUTTON_H
#define MALENA_CIRCLEBUTTON_H

#pragma once

#include <Malena/Core/malena_export.h>
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
    class MALENA_API CircleButton : public Button<Circle, float>
    {
    public:
        using Button::Button;
#ifdef _WIN32
        ~CircleButton() override;
#endif
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

#endif // MALENA_CIRCLEBUTTON_H
