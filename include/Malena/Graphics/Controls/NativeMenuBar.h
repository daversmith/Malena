// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <Malena/Core/malena_export.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace ml
{
    /**
     * @brief Native OS menu bar (macOS NSMenu / Windows HMENU).
     *
     * Builds the platform menu from a simple C++ API, then attaches it to the
     * application on macOS (global menu bar) or to the SFML window on Windows.
     * On unsupported platforms, attach() is a no-op.
     *
     * ### Usage
     * @code
     * ml::NativeMenuBar menuBar;
     *
     * auto& file = menuBar.addMenu("File");
     * file.addItem("Open...", []{ openFile(); });
     * file.addItem("Save",    []{ saveFile(); });
     * file.addSeparator();
     * file.addItem("Quit",    []{ window.close(); });
     *
     * auto& edit = menuBar.addMenu("Edit");
     * edit.addItem("Undo", []{ undo(); });
     *
     * menuBar.attach();   // call once after building menus
     * @endcode
     *
     * @note attach() is a blocking configuration step; it does not suspend the
     *       event loop. On Windows, adding a menu bar slightly reduces the
     *       client-area height — account for this when sizing your content.
     */
    class MALENA_API NativeMenuBar
    {
    public:
        struct Item
        {
            enum class Type { Action, Separator };
            Type                  type;
            std::string           label;
            std::function<void()> action;
        };

        class MALENA_API Menu
        {
        public:
            /** @brief Add a clickable menu item with a callback. */
            Menu& addItem(const std::string& label, std::function<void()> action);

            /** @brief Add a horizontal separator line. */
            Menu& addSeparator();

            const std::string&        getLabel() const { return _label; }
            const std::vector<Item>&  getItems() const { return _items; }

        private:
            friend class NativeMenuBar;
            explicit Menu(std::string label);
            std::string        _label;
            std::vector<Item>  _items;
        };

        NativeMenuBar() = default;
        ~NativeMenuBar();

        NativeMenuBar(const NativeMenuBar&)            = delete;
        NativeMenuBar& operator=(const NativeMenuBar&) = delete;

        /** @brief Add a top-level menu and return it for chaining. */
        Menu& addMenu(const std::string& label);

        /** @brief Build and attach the menu bar to the OS. */
        void attach();

        /** @brief Remove the menu bar and release platform resources. */
        void detach();

    private:
        std::vector<std::unique_ptr<Menu>> _menus;
        void* _nativeHandle = nullptr; ///< NSMenu* (macOS) or HMENU (Windows)
        void* _targetList   = nullptr; ///< NSMutableArray* of action targets (macOS only)
    };

} // namespace ml
