// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_TAB_H
#define MALENA_TAB_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Core.h>
#include <Malena/Utilities/HasSetSize.h>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>

namespace ml
{
    /**
     * @brief A single tab entry for @c TabbedPanel.
     *
     * Can be built ahead of time and passed to @c TabbedPanel::addTab(Tab):
     * @code
     * ml::Tab tab("Notes", std::make_unique<ml::TextArea>());
     * tab.setCloseable(true);
     * tab.setIcon(&myIcon);
     * tabbedPanel.addTab(std::move(tab));
     * @endcode
     *
     * Or created inline via the @c TabbedPanel::addTab convenience overload:
     * @code
     * tabbedPanel.addTab("Notes", std::make_unique<ml::TextArea>());
     * @endcode
     *
     * @see TabbedPanel
     */
    class MALENA_API Tab
    {
    public:
        std::string                       label;
        std::unique_ptr<ml::Core>         content;
        std::function<void(sf::Vector2f)> resizeFn;
        const sf::Texture*                icon      = nullptr;
        bool                              closeable = false;
        float                             x         = 0.f;   ///< computed by TabbedPanel
        float                             width     = 0.f;   ///< computed by TabbedPanel

        /**
         * @brief Construct a tab with owned content.
         *
         * @tparam T        Any @c ml::Core-derived type.
         * @param label     Tab label text.
         * @param content   Owned content component. TabbedPanel takes ownership on @c addTab.
         * @param icon      Optional icon texture. Pass @c nullptr for none.
         * @param closeable Whether this tab shows a close button.
         */
        template<typename T>
        Tab(const std::string& label, std::unique_ptr<T> content,
            const sf::Texture* icon = nullptr, bool closeable = false)
        : label(label), icon(icon), closeable(closeable)
        {
            static_assert(std::is_base_of_v<ml::Core, T>,
                "[Malena] Tab content must derive from ml::Core");
            T* ptr = content.get();
            this->content = std::move(content);
            if constexpr (detail::has_setSize<T>::value)
                resizeFn = [ptr](sf::Vector2f sz){ ptr->setSize(sz); };
        }

        void setLabel(const std::string& l)    { label     = l; }
        void setIcon(const sf::Texture* icon_) { icon      = icon_; }
        void setCloseable(bool c)              { closeable = c; }
    };

} // namespace ml

#endif // MALENA_TAB_H
