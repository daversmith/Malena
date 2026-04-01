//
// TabbedPanel.h
//

#ifndef TABBEDPANEL_H
#define TABBEDPANEL_H

#pragma once

#include <Malena/Core/Component.h>
#include <Malena/Core/Core.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Resources/FontManager.h>
#include <Malena/Traits/Settings/TabbedPanelSettings.h>
#include <Malena/Traits/Theme/TabbedPanelTheme.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <type_traits>

namespace ml
{
    class TabbedPanelManifest : public ml::Manifest
    {
    public:
        enum class Flag  {};
        enum class State {};
    };

    /**
     * @brief A container that displays one panel at a time, selected by tabs.
     * @ingroup GraphicsControls
     *
     * Tabs can be positioned on any edge (TOP/BOTTOM/LEFT/RIGHT), support
     * optional icons and a close button per tab, and hidden panels are kept
     * alive in memory so their state is preserved between switches.
     *
     * ### Usage
     * @code
     * ml::TabbedPanel tabs;
     * tabs.setSize({600.f, 400.f});
     * tabs.setPosition({40.f, 100.f});
     *
     * // Add a tab whose content is any ml::Core component
     * ml::TextArea notes;
     * notes.setSize(tabs.contentSize());
     * addComponent(notes);
     * tabs.addTab("Notes", notes);
     *
     * // Tab with icon
     * tabs.addTab("Settings", settingsPanel, &settingsIcon);
     *
     * // Closeable tab
     * tabs.addTab("Log", logPanel, nullptr, true);
     *
     * // Callback when active tab changes
     * tabs.onTabChanged([](std::size_t index, const std::string& label){
     *     std::cout << "Active tab: " << label << "\n";
     * });
     *
     * // Callback when a tab is closed
     * tabs.onTabClosed([](std::size_t index, const std::string& label){
     *     std::cout << "Closed: " << label << "\n";
     * });
     *
     * addComponent(tabs);
     * @endcode
     *
     * @see TabbedPanelSettings, TabbedPanelTheme, TabbedPanelStyle
     */
    class TabbedPanel : public ComponentWith<TabbedPanelManifest>,
                        public TabbedPanelSettings,
                        public TabbedPanelTheme,
                        public Themeable
    {
    public:
        using Flag        = TabbedPanelManifest::Flag;
        using State       = TabbedPanelManifest::State;
        using TabPosition = TabbedPanelSettings::TabPosition;

    private:
        struct Tab
        {
            std::string        label;
            ml::Core*          content   = nullptr;
            const sf::Texture* icon      = nullptr;
            bool               closeable = false;
            float              x         = 0.f;  ///< computed position in strip
            float              width     = 0.f;  ///< computed width
        };

        std::vector<Tab>  _tabs;
        int               _activeIdx  = -1;
        int               _hoveredIdx = -1;

        sf::Vector2f _position = {0.f, 0.f};
        sf::Vector2f _size     = {400.f, 300.f};

        std::function<void(std::size_t, const std::string&)> _onTabChanged;
        std::function<void(std::size_t, const std::string&)> _onTabClosed;

        // ── Internal ──────────────────────────────────────────────────────────
        void computeTabLayout();
        int  hitTestStrip(const sf::Vector2f& wp) const;
        bool hitTestClose(int tabIdx, const sf::Vector2f& wp) const;

        // Geometry helpers
        sf::FloatRect stripRect()   const;  ///< tab bar area
        sf::FloatRect contentRect() const;  ///< content area
        sf::Vector2f  stripAxis()   const;  ///< {1,0} for H strips, {0,1} for V

        void drawStrip(sf::RenderTarget& target, const sf::RenderStates& states) const;
        void drawTab(sf::RenderTarget& target, const sf::RenderStates& states,
                     int idx, bool active, bool hovered) const;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit TabbedPanel(const sf::Font& font = FontManager<>::getDefault());

        // ── Apply ─────────────────────────────────────────────────────────────

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<TabbedPanelSettings, S>,
                "applySettings() requires TabbedPanelSettings");
            static_cast<TabbedPanelSettings&>(*this) = s;
            computeTabLayout();
        }

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<TabbedPanelTheme, T>,
                "applyTheme() requires TabbedPanelTheme");
            static_cast<TabbedPanelTheme&>(*this) = t;
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<TabbedPanelSettings, St> &&
                          std::is_base_of_v<TabbedPanelTheme, St>,
                "applyStyle() requires TabbedPanelSettings and TabbedPanelTheme");
            static_cast<TabbedPanelSettings&>(*this) = s;
            static_cast<TabbedPanelTheme&>(*this)    = s;
            computeTabLayout();
        }

        // ── Tab management ────────────────────────────────────────────────────

        /**
         * @brief Add a tab.
         *
         * The content component is NOT owned — register it with @c addComponent
         * separately so its events fire. Content is just hidden (not destroyed)
         * when another tab is selected, so state is fully preserved.
         *
         * @param label     Tab label text.
         * @param content   Any @c ml::Core component to display in the panel.
         * @param icon      Optional icon texture. Pass @c nullptr for none.
         * @param closeable Whether this tab has a close button. Defaults to
         *                  the panel's @c closeable setting.
         * @return Zero-based index of the new tab.
         */
        std::size_t addTab(const std::string& label,
                           ml::Core&          content,
                           const sf::Texture* icon      = nullptr,
                           bool               closeable = false);

        /**
         * @brief Remove a tab by index.
         *
         * The content component is NOT destroyed — only detached from the panel.
         * If the active tab is removed the panel selects the nearest remaining tab.
         */
        void removeTab(std::size_t index);

        /** @brief Programmatically select a tab by index. */
        void selectTab(std::size_t index);

        /** @brief Return the currently active tab index, or -1 if no tabs. */
        [[nodiscard]] int activeTab() const { return _activeIdx; }

        /** @brief Return the number of tabs. */
        [[nodiscard]] std::size_t tabCount() const { return _tabs.size(); }

        /**
         * @brief Return the content area size.
         *
         * Use this to size content components before adding them:
         * @code
         * myContent.setSize(tabs.contentSize());
         * tabs.addTab("Label", myContent);
         * @endcode
         */
        [[nodiscard]] sf::Vector2f contentSize() const;

        // ── Callbacks ─────────────────────────────────────────────────────────

        /** @brief Fired when the active tab changes. */
        void onTabChanged(std::function<void(std::size_t, const std::string&)> cb);

        /** @brief Fired when a tab is closed via the × button. */
        void onTabClosed(std::function<void(std::size_t, const std::string&)> cb);

        // ── Size / position ───────────────────────────────────────────────────

        void setSize(const sf::Vector2f& size);
        [[nodiscard]] sf::Vector2f getSize() const { return _size; }

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class TabbedPanelWith : public TabbedPanel, public Customizable<MANIFEST>
    { public: using TabbedPanel::TabbedPanel; };

} // namespace ml
#endif // TABBEDPANEL_H
