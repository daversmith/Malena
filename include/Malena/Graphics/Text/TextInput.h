#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#pragma once

#include <Malena/Graphics/Text/Cursor.h>
#include <Malena/Graphics/Sprites/Sprite.h>
#include <Malena/Graphics/Text/Typer.h>

namespace ml
{
    /**
     * @brief A ready-to-use single-line text input field.
      * @ingroup GraphicsText
     *
     * @c TextInput composes a @c Typer (the editable text content) and a
     * @c Cursor (the blinking caret) into a single @c Core-derived object.
     * It inherits @c Subscribable, @c Flaggable, and @c Positionable from
     * @c Core, so it participates in hit-testing, focus management, and the
     * manager update/draw loop.
     *
     * @c setPosition repositions both the @c Typer and the @c Cursor together,
     * keeping the caret aligned with the text at all times.
     *
     * ### Usage
     * @code
     * ml::TextInput input;
     * input.setPosition({100.f, 200.f});
     * addComponent(input);
     *
     * // Read the current value
     * input.onUpdate([&]{
     *     sf::String value = input.typer.getString();
     * });
     * @endcode
     *
     * ### Focus and state
     * @c TextInput receives keyboard input only when it has focus.
     * @c UIManager sets @c ml::Flag::FOCUSED automatically when the user
     * clicks on the field.
     *
     * @note @c enableState takes a @c Flaggable::State parameter in the
     *       current source. This appears to be a naming artefact — verify
     *       against the @c Flaggable and @c ml::Flag definitions if you
     *       encounter a compile error.
     *
     * @see Typer, Cursor, Text, Flaggable
     */
    class TextInput : public ml::Core
    {
        ml::Typer  typer;
        ml::Cursor cursor;

    protected:
        /**
         * @brief Draw both the @c Typer and the @c Cursor to the render target.
         *
         * @param target SFML render target.
         * @param states Current render states.
         */
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    public:
        TextInput();

        /**
         * @brief Set the world-space position of the input field.
         *
         * Repositions both the @c Typer text and the @c Cursor caret.
         *
         * @param position New position in world coordinates.
         */
        void setPosition(const sf::Vector2f& position) override;

        /**
         * @brief Return the current world-space position of the input field.
         * @return Position of the @c Typer origin in world coordinates.
         */
        sf::Vector2f getPosition() const override;

        /**
         * @brief Return the axis-aligned bounding box of the input field.
         *
         * Used by @c UIManager for click/focus detection.
         *
         * @return Bounding rectangle in world coordinates.
         */
        [[nodiscard]] sf::FloatRect getGlobalBounds() const override;

        /**
         * @brief Enable a specific framework state on the input field.
         *
         * @param state The @c Flaggable state to enable.
         *
         * @note The parameter type @c Flaggable::State may be a naming
         *       artefact. Verify against the current @c Flaggable definition.
         */
        // void enableState(Flaggable::State state);
    };

} // namespace ml

#endif // TEXTINPUT_H
