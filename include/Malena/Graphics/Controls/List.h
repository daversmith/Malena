//
// List.h
//

#ifndef LIST_H
#define LIST_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Component.h>
#include <Malena/Core/Core.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Resources/FontManager.h>
#include <Malena/Graphics/Controls/ListItem.h>
#include <Malena/Traits/Settings/ListSettings.h>
#include <Malena/Traits/Theme/ListTheme.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <type_traits>

namespace ml
{
    // ── ListManifest ──────────────────────────────────────────────────────────

    class MALENA_API ListManifest : public ml::Manifest
    {
    public:
        enum class Flag  {};
        enum class State {};
    };

    // ── List ──────────────────────────────────────────────────────────────────

    /**
     * @brief A vertically stacked list of rows with optional dividers and background.
     * @ingroup GraphicsControls
     *
     * @c List is a layout container that stacks rows vertically and optionally
     * separates them with dividers. Each row is either:
     *
     * - A @c ListItem created internally via @c addItem() — owned and styled by
     *   the list. This is the typical path for standard rows.
     * - Any @c ml::Core component passed to @c add() — not owned, not resized.
     *   Use this for custom rows, section headers, separators, etc.
     *
     * Style changes via @c applySettings(), @c applyTheme() or @c applyStyle()
     * automatically propagate to all internally owned @c ListItem rows.
     *
     * ### Usage — standard rows
     * @code
     * ml::List settings;
     * settings.setWidth(400.f);
     * settings.setPosition({40.f, 100.f});
     *
     * // Row with label only
     * settings.addItem("Wi-Fi");
     *
     * // Row with label + description
     * settings.addItem("Bluetooth", "Pair devices wirelessly");
     *
     * // Row with end-slot toggle
     * ml::PillToggle darkMode;
     * auto& item = settings.addItem("Dark mode");
     * item.setEnd(darkMode);
     *
     * // Row with start-slot icon and end-slot toggle
     * ml::Sprite   wifiIcon;
     * wifiIcon.setTexture(wifiTexture);
     * ml::PillToggle wifiToggle;
     * auto& wifi = settings.addItem("Wi-Fi");
     * wifi.setStart(wifiIcon);
     * wifi.setEnd(wifiToggle);
     *
     * addComponent(settings);
     * @endcode
     *
     * ### Usage — custom row component
     * @code
     * ml::Rectangle header;
     * header.setFillColor(sf::Color(50, 50, 50));
     * settings.add(header);      // not owned — do addComponent separately if needed
     * @endcode
     *
     * ### Nested lists
     * @code
     * ml::List sub;
     * sub.addItem("Option A");
     * sub.addItem("Option B");
     * settings.add(sub);         // sub draws indented via its own position
     * @endcode
     *
     * @see ListItem, ListSettings, ListTheme, ListStyle
     */
    class MALENA_API List : public ComponentWith<ListManifest>,
                 public ListSettings,
                 public ListTheme,
                 public Themeable
    {
    public:
        using Flag  = ListManifest::Flag;
        using State = ListManifest::State;

    private:
        // ── Row storage ───────────────────────────────────────────────────────
        struct Row
        {
            ml::Core*                  component = nullptr; ///< Always valid
            std::unique_ptr<ListItem>  owned;               ///< Non-null when List created the item
        };

        std::vector<Row>              _rows;
        std::vector<sf::RectangleShape> _dividers;

        // ── Layout ────────────────────────────────────────────────────────────
        sf::RectangleShape _background;
        sf::Vector2f       _position = {0.f, 0.f};
        float              _width    = 300.f;
        float              _indent   = 0.f;  ///< Current indent level (for nested lists)

        // ── Internal helpers ──────────────────────────────────────────────────
        void layout();
        void rebuildDividers();
        void applyThemeToOwnedItems();
        void applySettingsToOwnedItems();

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit List(const sf::Font& font = FontManager<>::getDefault());

        List(const List&)            = delete;
        List& operator=(const List&) = delete;

        // ── Apply ─────────────────────────────────────────────────────────────

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<ListSettings, S>,
                "applySettings() requires a type derived from ListSettings");
            static_cast<ListSettings&>(*this) = s;
            applySettingsToOwnedItems();
            layout();
        }

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<ListTheme, T>,
                "applyTheme() requires a type derived from ListTheme");
            static_cast<ListTheme&>(*this) = t;
            applyThemeToOwnedItems();
            layout();
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<ListSettings, St> &&
                          std::is_base_of_v<ListTheme, St>,
                "applyStyle() requires ListSettings and ListTheme");
            static_cast<ListSettings&>(*this) = s;
            static_cast<ListTheme&>(*this)    = s;
            applyThemeToOwnedItems();
            applySettingsToOwnedItems();
            layout();
        }

        // ── Adding rows ───────────────────────────────────────────────────────

        /**
         * @brief Create and own a @c ListItem with a label.
         *
         * Returns a reference to the created item so the caller can configure
         * slots (@c setStart, @c setEnd, @c onClick etc.) immediately after.
         *
         * @param label       Primary label text.
         * @param description Optional secondary description line. Pass @c ""
         *                    to omit.
         * @return Reference to the newly created @c ListItem.
         */
        ListItem& addItem(const std::string& label,
                          const std::string& description = "");

        /**
         * @brief Add any @c ml::Core component as a row.
         *
         * The component is NOT owned by the list — the caller manages its
         * lifetime. The list does not resize or restyle it; it simply
         * positions it in the row sequence.
         *
         * @param component Any @c ml::Core (including @c List for nesting).
         */
        void add(ml::Core& component);

        /** @brief Remove all rows. Owned @c ListItem objects are destroyed. */
        void clear();

        /** @brief Return the number of rows. */
        [[nodiscard]] std::size_t rowCount() const { return _rows.size(); }

        // ── Sizing ────────────────────────────────────────────────────────────

        /**
         * @brief Set the total width of the list.
         *
         * Owned @c ListItem rows are automatically resized to this width.
         * External rows added via @c add() are not resized.
         *
         * @param width Width in pixels.
         */
        void setWidth(float width);
        [[nodiscard]] float getWidth()       const { return _width; }
        [[nodiscard]] float getTotalHeight() const;

        // ── Indent (for nested lists) ─────────────────────────────────────────

        /**
         * @brief Set the current indentation offset in pixels.
         * Called automatically by a parent @c List when nesting.
         */
        void setIndentOffset(float offset) { _indent = offset; layout(); }

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    // ── ListWith ──────────────────────────────────────────────────────────────

    template<typename MANIFEST>
    class ListWith : public List, public Customizable<MANIFEST>
    { public: using List::List; };

} // namespace ml

#endif // LIST_H
