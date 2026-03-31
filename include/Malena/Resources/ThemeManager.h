//
// ThemeManager.h
//

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#pragma once

#include <Malena/Manifests/Theme.h>
#include <Malena/Manifests/ThemeTag.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Core/DeferredOperationsManager.h>
#include <vector>
#include <memory>
#include <algorithm>

namespace ml
{
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
     * Inherits @c DeferredOperationsManager so that @c subscribe() and
     * @c unsubscribe() calls made during a theme notification (e.g. a component
     * creating children inside @c onThemeApplied()) are safely deferred until
     * the current iteration completes.
     *
     * ### Switching themes at runtime
     * @code
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
     * @endcode
     *
     * @see Theme, Themeable, DarkTheme, LightTheme, DeferredOperationsManager
     */
    class ThemeManager : public DeferredOperationsManager<ThemeManager>
    {
    public:
        // ── Reading ───────────────────────────────────────────────────────────

        /**
         * @brief Return a const reference to the currently active theme.
         *
         * Safe to call at any time including during component construction.
         * Defaults to @c DarkTheme before any theme has been applied.
         */
        static const Theme& get();

        // ── Applying themes ───────────────────────────────────────────────────

        /**
         * @brief Apply a theme stored in a @c Manifest enum.
         *
         * Retrieves the theme registered under @p themeKey, sets it as the
         * active theme, and notifies all subscribed @c Themeable components.
         *
         * @tparam MANIFEST  A @c Manifest subclass declaring a @c Themes enum.
         * @param  themeKey  Enum value identifying which theme to apply.
         *
         * @code
         * ml::ThemeManager::apply<GameManifest>(GameManifest::Themes::Home);
         * @endcode
         */
        /**
         * @brief Apply a theme registered via @c Manifest::set().
         *
         * Retrieves the theme stored in @c ThemeStore by @c Manifest::set(),
         * casts it back to @c Theme, and notifies all subscribed @c Themeable
         * components.
         *
         * @tparam MANIFEST  A @c Manifest subclass declaring a @c Themes enum.
         * @param  themeKey  Enum value identifying which theme to apply.
         *
         * @code
         * ml::ThemeManager::apply<GameManifest>(GameManifest::Themes::Home);
         * @endcode
         */
        template<typename MANIFEST>
        static void apply(typename MANIFEST::Themes themeKey)
        {
            const ThemeTag& tag = Manifest::getTheme(themeKey);
            applyTheme(static_cast<const Theme&>(tag));
        }

        /**
         * @brief Apply a theme instance directly without a manifest.
         *
         * @tparam T    A type derived from @c Theme.
         * @param  theme The theme instance to apply. Copied internally.
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

        // ── Subscription (called by Themeable — not for user code) ────────────

        /**
         * @brief Subscribe a component to theme change notifications.
         *
         * Called automatically by the @c Themeable constructor.
         * Safe to call during a theme notification — deferred if necessary.
         */
        static void subscribe(Themeable* component);

        /**
         * @brief Unsubscribe a component from theme change notifications.
         *
         * Called automatically by the @c Themeable destructor.
         * Safe to call during a theme notification — deferred if necessary.
         */
        static void unsubscribe(Themeable* component);

        // ── Lifecycle ─────────────────────────────────────────────────────────

        /**
         * @brief Shut down the theme system.
         *
         * Called automatically by @c ApplicationBase::~ApplicationBase().
         * Clears all subscribers and discards any pending deferred operations.
         * After this call, @c subscribe() and @c unsubscribe() are no-ops.
         */
        static void shutdown();

    private:
        static void applyTheme(const Theme& theme);

        /**
         * @brief Iterate subscribers and call @c onThemeApplied on each.
         *
         * Wrapped in @c beginBusy() / @c endBusy() so that any
         * subscribe/unsubscribe calls made during notification are safely
         * deferred and applied once iteration completes.
         */
        static void notify();

        inline static std::unique_ptr<Theme>  _active      = std::make_unique<DarkTheme>();
        inline static std::vector<Themeable*> _subscribers = {};
        inline static bool                    _destroyed   = false;
    };

} // namespace ml

#endif // THEMEMANAGER_H
