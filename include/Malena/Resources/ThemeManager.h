//
// ThemeManager.h
//

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#pragma once

#include <Malena/Manifests/Theme.h>
#include <Malena/Manifests/Manifest.h>
#include <vector>
#include <memory>
#include <algorithm>

namespace ml
{
    // Forward declare so Themeable can be a subscriber without a circular include
    class Themeable;

    // ── ThemeManager ─────────────────────────────────────────────────────────

    /**
     * @brief Global manager for the active @c Theme.
     * @ingroup Resources
     *
     * @c ThemeManager holds one active @c Theme at a time and notifies all
     * subscribed @c Themeable components when it changes. Components subscribe
     * automatically by inheriting @c Themeable — no manual registration needed.
     *
     * ### Switching themes at runtime
     * @code
     * // All Themeable components update automatically
     * ml::ThemeManager::apply<GameManifest>(GameManifest::Themes::GameOver);
     * @endcode
     *
     * ### Reading the active theme
     * @code
     * const ml::Theme& t = ml::ThemeManager::get();
     * sf::Color primary = t.primary;
     * @endcode
     *
     * ### Setting a theme directly (without a manifest)
     * @code
     * ml::ThemeManager::set(ml::DarkTheme());
     * ml::ThemeManager::set(NeonTheme());
     * @endcode
     *
     * @see Theme, Themeable, DarkTheme, LightTheme
     */
    class ThemeManager
    {
    public:
        // ── Reading the active theme ──────────────────────────────────────────

        /**
         * @brief Return a const reference to the currently active theme.
         *
         * Safe to call at any time, including during component construction.
         * Returns the @c DarkTheme default if no theme has been applied yet.
         */
        static const Theme& get()
        {
            return *_active;
        }

        // ── Applying themes ───────────────────────────────────────────────────

        /**
         * @brief Apply a theme stored in a @c Manifest enum.
         *
         * Retrieves the theme registered under @p themeKey from the manifest,
         * sets it as the active theme, and notifies all subscribed
         * @c Themeable components.
         *
         * @tparam MANIFEST  A @c Manifest subclass that declares a @c Themes enum.
         * @param  themeKey  The enum value identifying the theme to apply.
         *
         * @code
         * ml::ThemeManager::apply<GameManifest>(GameManifest::Themes::Home);
         * @endcode
         */
        template<typename MANIFEST>
        static void apply(typename MANIFEST::Themes themeKey)
        {
            const Theme& t = Manifest::getConfig<
                typename MANIFEST::Themes, Theme>(themeKey);
            applyTheme(t);
        }

        /**
         * @brief Apply a theme instance directly without a manifest.
         *
         * Useful for prototyping or when the theme is constructed inline.
         * Notifies all subscribed @c Themeable components.
         *
         * @param theme  The theme to apply. Copied internally.
         *
         * @code
         * ml::ThemeManager::set(NeonTheme());
         * @endcode
         */
        template<typename T>
        static void set(const T& theme)
        {
            static_assert(std::is_base_of_v<Theme, T>,
                "ThemeManager::set() requires a type derived from ml::Theme");
            _active = std::make_unique<T>(theme);
            notify();
        }

        // ── Subscription (called by Themeable, not by users) ──────────────────

        /**
         * @brief Subscribe a @c Themeable component to theme change notifications.
         *
         * Called automatically by the @c Themeable constructor.
         * Do not call this manually.
         *
         * @param component  The component to subscribe.
         */
        static void subscribe(Themeable* component)
        {
            if (std::find(_subscribers.begin(),
                          _subscribers.end(), component) == _subscribers.end())
                _subscribers.push_back(component);
        }

        /**
         * @brief Unsubscribe a @c Themeable component from theme change notifications.
         *
         * Called automatically by the @c Themeable destructor.
         * Do not call this manually.
         *
         * @param component  The component to unsubscribe.
         */
        static void unsubscribe(Themeable* component)
        {
            _subscribers.erase(
                std::remove(_subscribers.begin(),
                            _subscribers.end(), component),
                _subscribers.end());
        }

    private:
        // ── Internal ──────────────────────────────────────────────────────────

        static void applyTheme(const Theme& theme)
        {
            // Store a copy — we own the active theme
            // Use DarkTheme as the concrete type when storing a base Theme
            _active = std::make_unique<Theme>(theme);
            notify();
        }

        static void notify();  // defined in ThemeManager.cpp — avoids
                               // including Themeable.h here (breaks circular dep)

        // ── State ─────────────────────────────────────────────────────────────

        inline static std::unique_ptr<Theme>  _active      = std::make_unique<DarkTheme>();
        inline static std::vector<Themeable*> _subscribers = {};
    };

} // namespace ml

#endif // THEMEMANAGER_H
