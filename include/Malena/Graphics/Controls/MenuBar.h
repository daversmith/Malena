//
// MenuBar.h
//

#ifndef MALENA_MENUBAR_H
#define MALENA_MENUBAR_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Resources/FontManager.h>
#include <Malena/Graphics/Controls/MenuItem.h>
#include <Malena/Graphics/Controls/List.h>
#include <Malena/Graphics/Controls/ListItem.h>
#include <Malena/Graphics/Text/Text.h>
#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Traits/Settings/MenuBarSettings.h>
#include <Malena/Traits/Theme/MenuBarTheme.h>
#include <Malena/Traits/Theme/ListItemTheme.h>
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
    class MALENA_API MenuBarManifest : public ml::Manifest
    {
    public:
        enum class Flag  { OPEN };
        enum class State { IDLE, OPEN };
    };

    /**
     * @brief A desktop-style horizontal menu bar with dropdown panels.
     * @ingroup GraphicsControls
     *
     * @c MenuBar auto-fills the window width and sits at y=0 by default.
     * Each top-level entry opens a dropdown containing @c MenuItem rows
     * that support labels, shortcuts, icons, checkmarks, separators, and submenus.
     *
     * ### Usage
     * @code
     * ml::MenuBar bar;
     *
     * bar.addMenu("File", {
     *     ml::MenuItem::item("New",   [&]{ newFile(); },  "Ctrl+N"),
     *     ml::MenuItem::item("Open",  [&]{ openFile(); }, "Ctrl+O"),
     *     ml::MenuItem::sep(),
     *     ml::MenuItem::item("Save",  [&]{ save(); },     "Ctrl+S"),
     * });
     *
     * bar.addMenu("Edit", {
     *     ml::MenuItem::item("Undo", [&]{ undo(); }, "Ctrl+Z"),
     *     ml::MenuItem::sep(),
     *     ml::MenuItem::chk("Word wrap", wordWrap),
     *     ml::MenuItem::sub("Encoding", {
     *         ml::MenuItem::item("UTF-8"),
     *         ml::MenuItem::item("Latin-1"),
     *     }),
     * });
     *
     * addComponent(bar);
     * @endcode
     *
     * @see MenuItem, MenuBarSettings, MenuBarTheme, MenuBarStyle
     */
    class MALENA_API MenuBar : public ComponentWith<MenuBarManifest>,
                    public MenuBarSettings,
                    public MenuBarTheme,
                    public Themeable
    {
    public:
        using Flag  = MenuBarManifest::Flag;
        using State = MenuBarManifest::State;

    private:
        struct MenuEntry
        {
            std::string           label;
            std::vector<MenuItem> items;
            float                 labelX = 0.f; ///< computed x position in bar
            float                 labelW = 0.f; ///< computed label width

            // List-based dropdown (off-screen when closed)
            std::unique_ptr<ml::List> dropdown;

            // Parallel to items — null for separators
            std::vector<ml::ListItem*>              listItems;
            std::vector<std::unique_ptr<ml::Text>>  checkmarkTexts; ///< start slot
            std::vector<std::unique_ptr<ml::Text>>  endTexts;       ///< shortcut or arrow

            // Separator helper objects (kept alive for list layout)
            std::vector<std::unique_ptr<ml::Core>> separatorRects;
        };

        std::vector<MenuEntry> _entries;
        int  _openEntry   = -1;  ///< which top-level entry has its dropdown open
        int  _hoveredItem = -1;  ///< hovered row within open dropdown
        int  _openSubmenu = -1;  ///< hovered item that has a submenu
        sf::Vector2f _position  = {0.f, 0.f};
        float        _width     = 0.f;  ///< resolved bar width

        /// All data from a replaced dropdown, kept alive until the next UPDATE
        /// frame. The List's ListItems hold raw pointers to the Text objects in
        /// checkmarkTexts/endTexts; destroying either side first causes a crash
        /// when ThemeManager re-notifies the still-living List.
        struct OldDropdown
        {
            std::unique_ptr<ml::List>              dropdown;
            std::vector<std::unique_ptr<ml::Text>> checkmarkTexts;
            std::vector<std::unique_ptr<ml::Text>> endTexts;
            std::vector<std::unique_ptr<ml::Core>> separatorRects;
        };
        std::vector<OldDropdown> _pendingDelete;

        // ── Internal ──────────────────────────────────────────────────────────
        void  computeLayout();
        void  closeAll();
        void  buildDropdown(int entryIdx);
        int   hitTestBar(const sf::Vector2f& wp) const;
        int   hitTestDropdown(const sf::Vector2f& wp) const;
        float dropdownWidth(int entryIdx) const;
        float dropdownHeight(int entryIdx) const;
        float dropdownX(int entryIdx) const;
        float dropdownY() const;
        void  drawSubmenu(sf::RenderTarget& target,
                          const sf::RenderStates& states,
                          int entryIdx, int itemIdx) const;
        void  activateItem(int entryIdx, int itemIdx);

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit MenuBar(const sf::Font& font = FontManager<>::getDefault());

        MenuBar(const MenuBar&)            = delete;
        MenuBar& operator=(const MenuBar&) = delete;

        // ── Apply ─────────────────────────────────────────────────────────────

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<MenuBarSettings, S>,
                "applySettings() requires MenuBarSettings");
            static_cast<MenuBarSettings&>(*this) = s;
            computeLayout();
        }

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<MenuBarTheme, T>,
                "applyTheme() requires MenuBarTheme");
            static_cast<MenuBarTheme&>(*this) = t;
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<MenuBarSettings, St> &&
                          std::is_base_of_v<MenuBarTheme, St>,
                "applyStyle() requires MenuBarSettings and MenuBarTheme");
            static_cast<MenuBarSettings&>(*this) = s;
            static_cast<MenuBarTheme&>(*this)    = s;
            computeLayout();
        }

        // ── Menu management ───────────────────────────────────────────────────

        /** @brief Add a top-level menu entry with its dropdown items. */
        void addMenu(const std::string& label, std::vector<MenuItem> items);

        /** @brief Replace the items of an existing menu by index. */
        void setMenuItems(std::size_t menuIndex, std::vector<MenuItem> items);

        /** @brief Set checked state on a specific item. */
        void setItemChecked(std::size_t menuIndex, std::size_t itemIndex, bool checked);

        /** @brief Enable or disable a specific item. */
        void setItemEnabled(std::size_t menuIndex, std::size_t itemIndex, bool enabled);

        /** @brief Close all open dropdowns programmatically. */
        void closeMenus();

        /** @brief Return the number of top-level menu entries. */
        [[nodiscard]] std::size_t menuCount() const { return _entries.size(); }

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class MenuBarWith : public MenuBar, public Customizable<MANIFEST>
    { public: using MenuBar::MenuBar; };

} // namespace ml
#endif // MALENA_MENUBAR_H
