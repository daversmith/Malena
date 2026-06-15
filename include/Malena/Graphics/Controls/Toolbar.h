//
// Toolbar.h
//

#ifndef MALENA_TOOLBAR_H
#define MALENA_TOOLBAR_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Component.h>
#include <Malena/Core/Core.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Resources/FontManager.h>
#include <Malena/Traits/Settings/ToolbarSettings.h>
#include <Malena/Traits/Theme/ToolbarTheme.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <type_traits>

namespace ml
{
    class MALENA_API ToolbarManifest : public ml::Manifest
    {
    public:
        enum class Flag  {};
        enum class State {};
    };

    /**
     * @brief A horizontal or vertical strip of buttons, separators, and components.
     * @ingroup GraphicsControls
     *
     * Items can be added three ways:
     *
     * | Method | Result |
     * |--------|--------|
     * | @c addButton(label, action) | Creates an owned text button |
     * | @c addButton(label, icon, action) | Creates an owned icon+label button |
     * | @c addSeparator() | Thin visual divider line |
     * | @c add(component) | Any external @c ml::Core — not owned, not resized |
     *
     * All interaction (hover highlight, click) is handled by the toolbar itself.
     * External components added via @c add() retain their own event subscriptions.
     *
     * ### Usage
     * @code
     * ml::Toolbar toolbar;
     * toolbar.setPosition({0.f, 30.f}); // below a MenuBar
     *
     * toolbar.addButton("New",  [&]{ newFile(); });
     * toolbar.addButton("Open", [&]{ openFile(); });
     * toolbar.addButton("Save", [&]{ save(); });
     * toolbar.addSeparator();
     * toolbar.addButton("Undo", [&]{ undo(); });
     * toolbar.addButton("Redo", [&]{ redo(); });
     * toolbar.addSeparator();
     *
     * // Embed any component as a visual toolbar item.
     * // add() handles positioning; addComponent() registers it for events.
     * // Both calls are required for interactive components.
     * ml::TextInput search;
     * search.setSize({200.f, 28.f});
     * search.setPlaceholder("Search...");
     * toolbar.add(search);      // positions in toolbar flow
     * addComponent(search);     // enables click / hover / update events
     *
     * addComponent(toolbar);
     * @endcode
     *
     * @see ToolbarSettings, ToolbarTheme, ToolbarStyle
     */
    class MALENA_API Toolbar : public ComponentWith<ToolbarManifest>,
                    public ToolbarSettings,
                    public ToolbarTheme,
                    public Themeable
    {
    public:
        using Flag        = ToolbarManifest::Flag;
        using State       = ToolbarManifest::State;
        using Orientation = ToolbarSettings::Orientation;
        using Overflow    = ToolbarSettings::Overflow;

    private:
        struct Item
        {
            ml::Core*                 component = nullptr; ///< always valid
            std::unique_ptr<ml::Core> owned;               ///< set for created buttons
            std::function<void()>     action;
            std::string               label;
            bool                      separator = false;
            bool                      hovered   = false;
            bool                      enabled   = true;
            bool                      selected  = false;

            Item() = default;
            Item(Item&&) = default;
            Item& operator=(Item&&) = default;
        };

        std::vector<Item> _items;
        sf::Vector2f      _position       = {0.f, 0.f};
        float             _barLength      = 0.f;  ///< resolved bar length (width if H, height if V)
        float             _scrollOffsetX  = 0.f;  ///< horizontal scroll offset (Overflow::SCROLL)
        float             _totalItemsLen  = 0.f;  ///< total item span along the bar axis
        mutable int       _rowCount       = 1;    ///< rows used (Overflow::WRAP)
        int               _hoveredIdx     = -1;

        void layout();
        void drawSeparator(sf::RenderTarget& target,
                           const sf::RenderStates& states,
                           const sf::Vector2f& pos) const;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit Toolbar(const sf::Font& font = FontManager<>::getDefault());

        Toolbar(const Toolbar&)            = delete;
        Toolbar& operator=(const Toolbar&) = delete;

        // ── Apply ─────────────────────────────────────────────────────────────

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<ToolbarSettings, S>,
                "applySettings() requires ToolbarSettings");
            static_cast<ToolbarSettings&>(*this) = s;
            layout();
        }

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<ToolbarTheme, T>,
                "applyTheme() requires ToolbarTheme");
            static_cast<ToolbarTheme&>(*this) = t;
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<ToolbarSettings, St> &&
                          std::is_base_of_v<ToolbarTheme, St>,
                "applyStyle() requires ToolbarSettings and ToolbarTheme");
            static_cast<ToolbarSettings&>(*this) = s;
            static_cast<ToolbarTheme&>(*this)    = s;
            layout();
        }

        // ── Adding items ──────────────────────────────────────────────────────

        /** @brief Add a text-only button. Returns the item index. */
        std::size_t addButton(const std::string& label,
                              std::function<void()> action = {});

        /** @brief Update the label of an owned button by item index. */
        void setItemLabel(std::size_t index, const std::string& label);

        /** @brief Enable or disable an owned button by item index. */
        void setItemEnabled(std::size_t index, bool enabled);

        /** @brief Set or clear the persistent selected highlight on an owned button. */
        void setItemSelected(std::size_t index, bool selected);

        /**
         * @brief Embed any @c ml::Core as a positioned toolbar item.
         *
         * This handles **visual placement only** — the component is placed in the
         * item flow and repositioned whenever the toolbar moves. It is NOT owned.
         *
         * To also receive framework events (click, hover, update), you must also
         * register the component with the host's component manager:
         *
         * @code
         * toolbar.add(myToggle);      // place in toolbar
         * addComponent(myToggle);     // enable events
         * @endcode
         *
         * For display-only items that need no events, @c add() alone is sufficient.
         *
         * @c setEnabled() on the toolbar propagates to all embedded components,
         * including those added via this method.
         *
         * @param component Any @c ml::Core. Not owned — caller manages lifetime.
         */
        void add(ml::Core& component);

        /** @brief Const overload — forwards to the non-const @c add(). */
        void add(const ml::Core& component);

        /** @brief Add a visual separator. */
        void addSeparator();

        /** @brief Remove all items. */
        void clear();

        /** @brief Return the number of items (including separators). */
        [[nodiscard]] std::size_t itemCount() const { return _items.size(); }

        // ── Sizing ────────────────────────────────────────────────────────────

        /**
         * @brief Override the bar length (width for H, height for V).
         * By default fills the window width.
         */
        void setBarLength(float length);
        [[nodiscard]] float getBarLength() const { return _barLength; }
        [[nodiscard]] float getBarThickness() const;  ///< height (H) or width (V)

        /**
         * @brief Extent of the laid-out items along the bar (width for H,
         * height for V), measured from the bar origin past the last item.
         * Useful for placing sibling controls right after a toolbar whose bar
         * spans the full window.
         */
        [[nodiscard]] float getContentExtent() const;

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;

        void setEnabled(bool enabled)       override;
        void setParentEnabled(bool enabled) override;
    };

    template<typename MANIFEST>
    class ToolbarWith : public Toolbar, public Customizable<MANIFEST>
    { public: using Toolbar::Toolbar; };

} // namespace ml
#endif // MALENA_TOOLBAR_H
