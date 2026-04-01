//
// SplitPanel.h
//

#ifndef SPLITPANEL_H
#define SPLITPANEL_H

#pragma once

#include <Malena/Core/Component.h>
#include <Malena/Core/Core.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Resources/FontManager.h>
#include <Malena/Traits/Settings/SplitPanelSettings.h>
#include <Malena/Traits/Theme/SplitPanelTheme.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <vector>
#include <type_traits>

namespace ml
{
    class SplitPanelManifest : public ml::Manifest
    {
    public:
        enum class Flag  {};
        enum class State { IDLE, DIVIDER_HOVER, DIVIDER_DRAG };
    };

    /**
     * @brief A resizable multi-pane container split horizontally or vertically.
     * @ingroup GraphicsControls
     *
     * Supports any number of panes separated by draggable dividers. Each pane
     * holds one @c ml::Core component. Dragging a divider resizes its two
     * adjacent panes while respecting per-pane minimum sizes.
     *
     * ### Usage — two panes
     * @code
     * ml::SplitPanel split;
     * split.setSize({800.f, 600.f});
     * split.setPosition({40.f, 100.f});
     *
     * ml::List sidebar;
     * ml::TextArea editor;
     * addComponent(sidebar);
     * addComponent(editor);
     *
     * split.addPane(sidebar, 220.f);   // 220px initial size
     * split.addPane(editor);           // fills remaining space
     *
     * addComponent(split);
     * @endcode
     *
     * ### Vertical split
     * @code
     * split.setOrientation(ml::SplitPanelSettings::Orientation::VERTICAL);
     * split.addPane(header, 60.f);
     * split.addPane(content);
     * split.addPane(footer,  40.f);
     * @endcode
     *
     * ### Per-pane min/max sizes
     * @code
     * split.setPaneMinSize(0, 150.f);  // sidebar never smaller than 150
     * split.setPaneMinSize(1, 200.f);  // editor never smaller than 200
     * @endcode
     *
     * ### Callback when a divider is moved
     * @code
     * split.onDividerMoved([](std::size_t dividerIdx, float newPosition){
     *     std::cout << "Divider " << dividerIdx
     *               << " at " << newPosition << "px\n";
     * });
     * @endcode
     *
     * @see SplitPanelSettings, SplitPanelTheme, SplitPanelStyle
     */
    class SplitPanel : public ComponentWith<SplitPanelManifest>,
                       public SplitPanelSettings,
                       public SplitPanelTheme,
                       public Themeable
    {
    public:
        using Flag        = SplitPanelManifest::Flag;
        using State       = SplitPanelManifest::State;
        using Orientation = SplitPanelSettings::Orientation;
        using HandleStyle = SplitPanelSettings::HandleStyle;

    private:
        struct Pane
        {
            ml::Core* content  = nullptr;
            float     size     = 0.f;   ///< current size along the split axis
            float     minSize  = 0.f;   ///< 0 = use global minPaneSize
            float     maxSize  = 0.f;   ///< 0 = unlimited
        };

        std::vector<Pane> _panes;

        sf::Vector2f _position = {0.f, 0.f};
        sf::Vector2f _size     = {400.f, 300.f};

        int   _hoveredDivider = -1;
        int   _draggingDiv    = -1;
        float _dragStartMouse = 0.f;
        float _dragStartSizeA = 0.f;
        float _dragStartSizeB = 0.f;
        bool  _prevMouseDown  = false;

        std::function<void(std::size_t, float)> _onDividerMoved;

        // ── Internal ──────────────────────────────────────────────────────────
        float totalAvailable()           const;
        float dividerPos(int divIdx)     const;
        int   hitTestDivider(const sf::Vector2f& wp) const;
        void  layoutPanes();
        void  distributeSizes();
        void  drawDivider(sf::RenderTarget& target,
                          const sf::RenderStates& states,
                          int divIdx, bool hovered, bool dragging) const;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit SplitPanel(Orientation orientation = Orientation::HORIZONTAL,
                            const sf::Font& font = FontManager<>::getDefault());

        // ── Apply ─────────────────────────────────────────────────────────────

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<SplitPanelSettings, S>,
                "applySettings() requires SplitPanelSettings");
            static_cast<SplitPanelSettings&>(*this) = s;
            layoutPanes();
        }

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<SplitPanelTheme, T>,
                "applyTheme() requires SplitPanelTheme");
            static_cast<SplitPanelTheme&>(*this) = t;
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<SplitPanelSettings, St> &&
                          std::is_base_of_v<SplitPanelTheme, St>,
                "applyStyle() requires SplitPanelSettings and SplitPanelTheme");
            static_cast<SplitPanelSettings&>(*this) = s;
            static_cast<SplitPanelTheme&>(*this)    = s;
            layoutPanes();
        }

        // ── Pane management ───────────────────────────────────────────────────

        /**
         * @brief Add a pane.
         *
         * The content component is NOT owned. Register it with @c addComponent
         * separately so its events fire.
         *
         * @param content     Component to display in this pane.
         * @param initialSize Initial size along the split axis in pixels.
         *                    Pass 0 to distribute remaining space equally.
         */
        void addPane(ml::Core& content, float initialSize = 0.f);

        /** @brief Set the minimum size of a pane by index. */
        void setPaneMinSize(std::size_t index, float minSize);

        /** @brief Set the maximum size of a pane by index. */
        void setPaneMaxSize(std::size_t index, float maxSize);

        /** @brief Programmatically set a divider position. */
        void setDividerPosition(std::size_t dividerIndex, float position);

        /** @brief Return the number of panes. */
        [[nodiscard]] std::size_t paneCount() const { return _panes.size(); }

        // ── Callback ─────────────────────────────────────────────────────────

        /** @brief Fired whenever a divider is moved (by drag or API). */
        void onDividerMoved(std::function<void(std::size_t, float)> cb);

        // ── Size / position ───────────────────────────────────────────────────

        void setSize(const sf::Vector2f& size);
        [[nodiscard]] sf::Vector2f getSize() const { return _size; }

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class SplitPanelWith : public SplitPanel, public Customizable<MANIFEST>
    { public: using SplitPanel::SplitPanel; };

} // namespace ml
#endif // SPLITPANEL_H
