// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_COLORBUTTON_H
#define MALENA_COLORBUTTON_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Graphics/Base/Graphic.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <functional>

namespace ml
{
    /**
     * @brief A small clickable swatch filled with a color — e.g. a toolbar
     *        "text color" button that shows the last-chosen color.
     * @ingroup GraphicsControls
     *
     * Self-contained (mouse-poll), so it works in composites. Set the color with
     * @c setColor and react to clicks with @c onClick.
     */
    class MALENA_API ColorButton : public Graphic<sf::RectangleShape>
    {
    public:
        ColorButton();

        void setColor(const sf::Color& c);
        [[nodiscard]] sf::Color color() const { return _color; }

        /** Optional: drive the displayed color live each frame (e.g. show the
         *  color at a text cursor). Overrides @c setColor while set. */
        void setColorProvider(std::function<sf::Color()> provider);

        void onClick(std::function<void()> cb);

        void setSize(const sf::Vector2f& size);
        [[nodiscard]] sf::Vector2f getSize() const { return _size; }

        void setPosition(const sf::Vector2f& pos) override;
        [[nodiscard]] sf::Vector2f  getPosition()     const override;
        [[nodiscard]] sf::FloatRect getGlobalBounds() const override;

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    private:
        sf::Color    _color { 30, 30, 30 };
        sf::Vector2f _pos   { 0.f, 0.f };
        sf::Vector2f _size  { 26.f, 26.f };
        bool         _prevMouseDown = false;
        mutable bool _hover = false;
        std::function<void()>        _onClick;
        std::function<sf::Color()>   _colorProvider;
    };

} // namespace ml

#endif // MALENA_COLORBUTTON_H
