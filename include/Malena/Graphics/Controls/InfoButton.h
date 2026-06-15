// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_INFOBUTTON_H
#define MALENA_INFOBUTTON_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Graphics/Base/Graphic.h>
#include <Malena/Resources/FontManager.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <string>

namespace ml
{
    /**
     * @brief A small circular "ⓘ" badge that reveals a help popover on click.
     * @ingroup GraphicsControls
     *
     * Use it next to a field label to tuck longer instructions out of the way:
     * the form stays clean, and the help text appears in a popover only when the
     * student/teacher clicks the badge. Click the badge again (or anywhere outside
     * the popover) to dismiss it.
     *
     * Poll-based like the other lightweight pop-up controls; it respects modal
     * exclusive ownership (won't react from behind a dialog). Its
     * @c getGlobalBounds reports only the badge, so layouts size around the badge,
     * not the (transient) popover. Register it AFTER the fields it annotates so the
     * popover draws on top.
     *
     * @code
     * ml::InfoButton info;
     * info.setText("Wrap each blank's answer in {{ }}; separate alternatives with |.");
     * info.setPosition({ x, y });
     * addComponent(info);   // add last so the popover draws over the form
     * @endcode
     */
    class MALENA_API InfoButton : public Graphic<sf::CircleShape>
    {
    public:
        explicit InfoButton(const sf::Font& font = FontManager<>::getDefault());

        void setText(const std::string& help);     // popover body text
        void setRadius(float r);
        void setPopoverWidth(float w);

        void open();
        void close();
        void toggle();
        [[nodiscard]] bool isOpen() const { return _open; }

        // ── Positionable ──────────────────────────────────────────────────────
        void          setPosition(const sf::Vector2f& pos) override;
        [[nodiscard]] sf::Vector2f  getPosition()     const override;
        [[nodiscard]] sf::FloatRect getGlobalBounds() const override;   // badge only

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    private:
        const sf::Font* _font;
        std::string     _text;
        float           _radius   = 9.f;
        float           _popWidth = 260.f;
        sf::Vector2f    _pos      = { 0.f, 0.f };
        bool            _open     = false;
        bool            _prevDown = false;
    };

} // namespace ml

#endif // MALENA_INFOBUTTON_H
