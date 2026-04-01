//
// MenuItem.h
//

#ifndef MENUITEM_H
#define MENUITEM_H

#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <functional>
#include <string>
#include <vector>

namespace ml
{
    /**
     * @brief A single entry in a @c MenuBar dropdown or submenu.
     *
     * Use the static factory helpers for clean construction:
     *
     * @code
     * ml::MenuBar bar;
     * bar.addMenu("File", {
     *     ml::MenuItem::item("New",   [&]{ newFile(); },  "Ctrl+N"),
     *     ml::MenuItem::item("Open",  [&]{ openFile(); }, "Ctrl+O"),
     *     ml::MenuItem::sep(),
     *     ml::MenuItem::item("Save",  [&]{ save(); },     "Ctrl+S"),
     *     ml::MenuItem::sep(),
     *     ml::MenuItem::item("Quit",  [&]{ app.quit(); }, "Ctrl+Q"),
     * });
     *
     * bar.addMenu("Edit", {
     *     ml::MenuItem::item("Undo", [&]{ undo(); }, "Ctrl+Z"),
     *     ml::MenuItem::item("Redo", [&]{ redo(); }, "Ctrl+Y"),
     *     ml::MenuItem::sep(),
     *     ml::MenuItem::chk("Word wrap", wordWrap),
     *     ml::MenuItem::sub("Insert", {
     *         ml::MenuItem::item("Table"),
     *         ml::MenuItem::item("Image"),
     *     }),
     * });
     * @endcode
     *
     * @see MenuBar
     */
    struct MenuItem
    {
        std::string            label;
        std::string            shortcut;
        const sf::Texture*     icon      = nullptr;
        bool                   checked   = false;
        bool                   enabled   = true;
        bool                   separator = false;
        std::function<void()>  action;
        std::vector<MenuItem>  submenu;

        // ── Factory helpers ───────────────────────────────────────────────────

        /** @brief Standard clickable item. */
        static MenuItem item(const std::string& label,
                             std::function<void()> action = {},
                             const std::string& shortcut  = "",
                             const sf::Texture* icon      = nullptr)
        {
            MenuItem m;
            m.label    = label;
            m.action   = std::move(action);
            m.shortcut = shortcut;
            m.icon     = icon;
            return m;
        }

        /** @brief Checkable item that toggles a bool reference. */
        static MenuItem chk(const std::string& label,
                            bool& ref,
                            std::function<void()> onChange = {})
        {
            MenuItem m;
            m.label   = label;
            m.checked = ref;
            m.action  = [&ref, cb = std::move(onChange)]{
                ref = !ref;
                if (cb) cb();
            };
            return m;
        }

        /** @brief Item that opens a submenu. */
        static MenuItem sub(const std::string& label,
                            std::vector<MenuItem> items)
        {
            MenuItem m;
            m.label   = label;
            m.submenu = std::move(items);
            return m;
        }

        /** @brief Horizontal divider line. */
        static MenuItem sep()
        {
            MenuItem m;
            m.separator = true;
            return m;
        }

        /** @brief Greyed-out non-interactive label. */
        static MenuItem disabled(const std::string& label)
        {
            MenuItem m;
            m.label   = label;
            m.enabled = false;
            return m;
        }

        [[nodiscard]] bool hasSubmenu() const { return !submenu.empty(); }
    };

} // namespace ml

#endif // MENUITEM_H
