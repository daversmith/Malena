//
// ListItem.h
//

#ifndef LISTITEM_H
#define LISTITEM_H

#pragma once

#include <Malena/Core/Component.h>
#include <Malena/Core/Core.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Resources/FontManager.h>
#include <Malena/Traits/Settings/ListItemSettings.h>
#include <Malena/Traits/Theme/ListItemTheme.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <string>
#include <type_traits>

namespace ml
{
    // ── ListItemManifest ──────────────────────────────────────────────────────

    class ListItemManifest : public ml::Manifest
    {
    public:
        enum class Flag  { DISABLED };
        enum class State { IDLE, HOVERED, DISABLED };
    };

    // ── ListItem ──────────────────────────────────────────────────────────────

    /**
     * @brief A single row in a @c List with start, content and end slots.
     * @ingroup GraphicsControls
     *
     * Inspired by Ionic Framework's @c ion-item — each row has three optional
     * slots that accept any @c ml::Core component:
     *
     * | Slot    | Typical use                            |
     * |---------|----------------------------------------|
     * | start   | Icon, avatar, @c Checkbox, @c Radio    |
     * | content | Label + description (built-in) or any  @c Core |
     * | end     | @c Toggle, @c RectangleButton, badge   |
     *
     * Slot components are **not owned** by @c ListItem — the caller is
     * responsible for their lifetime. Do NOT call @c addComponent on them
     * separately; @c List handles drawing.
     *
     * ### Usage — built-in label
     * @code
     * ml::ListItem item;
     * item.setLabel("Dark mode");
     * item.setDescription("Reduces eye strain");
     *
     * ml::PillToggle toggle;
     * item.setEnd(toggle);
     *
     * list.add(item);   // add() accepts any ml::Core, including a pre-built ListItem
     * @endcode
     *
     * ### Usage — custom content slot
     * @code
     * ml::ListItem item;
     * ml::CheckboxGroup group;
     * item.setContent(group);    // replaces built-in label
     * list.add(item);
     * @endcode
     *
     * @see List, ListItemSettings, ListItemTheme, ListItemStyle
     */
    class ListItem : public ComponentWith<ListItemManifest>,
                     public ListItemSettings,
                     public ListItemTheme,
                     public Themeable
    {
    public:
        using Flag  = ListItemManifest::Flag;
        using State = ListItemManifest::State;

    private:
        // ── Slots — external references, never owned ──────────────────────────
        ml::Core* _start   = nullptr;
        ml::Core* _content = nullptr;   ///< Custom content (replaces label/desc)
        ml::Core* _end     = nullptr;

        // ── Built-in content ──────────────────────────────────────────────────
        sf::Text  _label;
        sf::Text  _description;
        bool      _hasDescription = false;
        bool      _hasCustomContent = false;

        // ── Layout ────────────────────────────────────────────────────────────
        sf::RectangleShape _background;
        sf::Vector2f       _position  = {0.f, 0.f};
        float              _width     = 300.f;

        // ── Callbacks ─────────────────────────────────────────────────────────
        std::function<void()> _onClickCb;

        // ── Internal ──────────────────────────────────────────────────────────
        void layout();
        void applyVisualState();
        float contentHeight() const;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit ListItem(const sf::Font& font = FontManager<>::getDefault());

        // ── Apply ─────────────────────────────────────────────────────────────

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<ListItemSettings, S>,
                "applySettings() requires a type derived from ListItemSettings");
            static_cast<ListItemSettings&>(*this) = s;
            layout();
        }

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<ListItemTheme, T>,
                "applyTheme() requires a type derived from ListItemTheme");
            static_cast<ListItemTheme&>(*this) = t;
            applyVisualState();
            layout();
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<ListItemSettings, St> &&
                          std::is_base_of_v<ListItemTheme, St>,
                "applyStyle() requires ListItemSettings and ListItemTheme");
            static_cast<ListItemSettings&>(*this) = s;
            static_cast<ListItemTheme&>(*this)    = s;
            applyVisualState();
            layout();
        }

        // ── Slots ─────────────────────────────────────────────────────────────

        /**
         * @brief Assign a component to the start (left) slot.
         *
         * Typical use: icon, @c Checkbox, @c RadioButton, avatar.
         * The component is not owned — do not @c addComponent it separately.
         */
        void setStart(ml::Core& component);

        /**
         * @brief Assign a component to the end (right) slot.
         *
         * Typical use: @c PillToggle, @c RectangleButton, badge.
         * The component is not owned — do not @c addComponent it separately.
         */
        void setEnd(ml::Core& component);

        /**
         * @brief Replace the built-in label/description with a custom component.
         *
         * When set, @c setLabel and @c setDescription have no visual effect.
         * The component is not owned — do not @c addComponent it separately.
         */
        void setContent(ml::Core& component);

        // ── Built-in content ──────────────────────────────────────────────────

        /** @brief Set the primary label text. */
        void setLabel(const std::string& text);
        [[nodiscard]] std::string getLabel() const;

        /**
         * @brief Set an optional secondary description line below the label.
         * Pass an empty string to remove.
         */
        void setDescription(const std::string& text);
        [[nodiscard]] std::string getDescription() const;

        // ── Click callback ────────────────────────────────────────────────────

        /** @brief Register a callback fired when the row is clicked. */
        void onClick(std::function<void()> callback);

        // ── State ─────────────────────────────────────────────────────────────

        void setEnabled(bool enabled);
        [[nodiscard]] bool isEnabled() const;

        // ── Width ─────────────────────────────────────────────────────────────

        /**
         * @brief Set the total row width.
         * Called automatically by @c List when the item is added.
         */
        void setWidth(float width);
        [[nodiscard]] float getWidth() const { return _width; }

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    // ── ListItemWith ──────────────────────────────────────────────────────────

    template<typename MANIFEST>
    class ListItemWith : public ListItem, public Customizable<MANIFEST>
    { public: using ListItem::ListItem; };

} // namespace ml

#endif // LISTITEM_H
