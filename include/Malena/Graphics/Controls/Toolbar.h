//
// Toolbar.h
//

#ifndef TOOLBAR_H
#define TOOLBAR_H

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
     * // Embed any component — e.g. a search TextInput
     * ml::TextInput search;
     * search.setSize({200.f, 28.f});
     * search.setPlaceholder("Search...");
     * toolbar.add(search);
     * addComponent(search);
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

            Item() = default;
            Item(Item&&) = default;
            Item& operator=(Item&&) = default;
        };

        std::vector<Item> _items;
        sf::Vector2f      _position  = {0.f, 0.f};
        float             _barLength = 0.f;  ///< resolved bar length (width if H, height if V)
        int               _hoveredIdx = -1;

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

        /** @brief Add a text-only button. */
        void addButton(const std::string& label,
                       std::function<void()> action = {});

        /**
         * @brief Add a component as a toolbar item.
         *
         * The component is NOT owned — register it with @c addComponent separately
         * so its events fire. The toolbar positions it; the component sizes itself.
         */
        void add(ml::Core& component);

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

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class ToolbarWith : public Toolbar, public Customizable<MANIFEST>
    { public: using Toolbar::Toolbar; };

} // namespace ml
#endif // TOOLBAR_H
