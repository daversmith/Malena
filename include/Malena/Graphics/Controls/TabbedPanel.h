//
// TabbedPanel.h
//

#ifndef TABBEDPANEL_H
#define TABBEDPANEL_H

#pragma once

#include <Malena/Core/malena_export.h>
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
#include <SFML/System/Vector2.hpp>
#include <Malena/Utilities/HasSetSize.h>

namespace ml
{
    class MALENA_API TabbedPanelManifest : public ml::Manifest
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
     * // addTab() takes ownership — content is drawn and resized by the panel
     * auto& notes = tabs.addTab("Notes", std::make_unique<ml::TextArea>());
     *
     * // Tab with icon
     * auto& settings = tabs.addTab("Settings",
     *                               std::make_unique<ml::Panel>(), &settingsIcon);
     *
     * // Closeable tab
     * auto& log = tabs.addTab("Log", std::make_unique<ml::TextArea>(), nullptr, true);
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
    class MALENA_API TabbedPanel : public ComponentWith<TabbedPanelManifest>,
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
            std::string                       label;
            std::unique_ptr<ml::Core>         content;
            std::function<void(sf::Vector2f)> resizeFn;   ///< calls setSize if supported
            const sf::Texture*                icon      = nullptr;
            bool                              closeable = false;
            float                             x         = 0.f;  ///< computed position in strip
            float                             width     = 0.f;  ///< computed width
        };

        std::vector<Tab>  _tabs;
        int               _activeIdx  = -1;
        int               _hoveredIdx = -1;

        sf::Vector2f _position = {0.f, 0.f};
        sf::Vector2f _size     = {400.f, 300.f};

        // Content whose unique_ptr was transferred out during event dispatch is
        // held here until the next onUpdate tick, preventing use-after-free when
        // the EventManager's CLICK loop continues past a just-closed tab.
        std::vector<std::unique_ptr<ml::Core>> _pendingDelete;

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
         * @brief Add a tab with owned content.
         *
         * The panel takes ownership of @p content. It is responsible for
         * drawing, resizing, and destroying it. If @c T has a @c setSize()
         * method, the panel will call it automatically whenever the content
         * area changes.
         *
         * @tparam T        Any @c ml::Core-derived type.
         * @param label     Tab label text.
         * @param content   Owned content component.
         * @param icon      Optional icon texture. Pass @c nullptr for none.
         * @param closeable Whether this tab has a close button.
         * @return Zero-based index of the new tab.
         */
        template<typename T>
        T& addTab(const std::string& label,
                  std::unique_ptr<T>  content,
                  const sf::Texture*  icon      = nullptr,
                  bool                closeable = false)
        {
            static_assert(std::is_base_of_v<ml::Core, T>,
                "addTab() content must derive from ml::Core");

            T* ptr = content.get();

            Tab tab;
            tab.label     = label;
            tab.content   = std::move(content);
            tab.icon      = icon;
            tab.closeable = closeable;

            if constexpr (detail::has_setSize<T>::value)
                tab.resizeFn = [ptr](sf::Vector2f sz){ ptr->setSize(sz); };

            _tabs.push_back(std::move(tab));
            computeTabLayout();

            if (_activeIdx < 0)
                selectTab(0);

            return *ptr;
        }

        /**
         * @brief Remove a tab by index.
         *
         * The content component is destroyed on the next update frame (deferred
         * to avoid use-after-free during event dispatch). If the active tab is
         * removed, the panel selects the nearest remaining tab.
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
         * Components with a @c setSize() method are resized automatically
         * whenever the content area changes. Call this when you need to query
         * the current bounds before constructing content:
         * @code
         * auto content = std::make_unique<ml::Panel>();
         * content->setSize(tabs.contentSize());  // optional — auto-resized anyway
         * tabs.addTab("Label", std::move(content));
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
