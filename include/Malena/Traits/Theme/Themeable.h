//
// Themeable.h
//

#ifndef THEMEABLE_H
#define THEMEABLE_H

#pragma once

#include <Malena/Traits/Base/Trait.h>
#include <Malena/Resources/ThemeManager.h>

namespace ml
{
    // ── Themeable ─────────────────────────────────────────────────────────────

    /**
     * @brief Trait that makes a component react to global theme changes.
     * @ingroup Traits
     *
     * Any component that inherits @c Themeable automatically subscribes to
     * @c ThemeManager on construction and unsubscribes on destruction. When
     * @c ThemeManager::apply() or @c ThemeManager::set() is called, every
     * subscribed component's @c onThemeApplied() is invoked.
     *
     * ### Adding Themeable to a component
     * @code
     * class PillToggle : public ComponentWith<PillToggleManifest>,
     *                    public PillSettings,
     *                    public PillTheme,
     *                    public Themeable
     * {
     * protected:
     *     void onThemeApplied(const Theme& theme) override
     *     {
     *         if (isThemeLocked()) return;
     *         PillTheme::applyFrom(theme);  // walks full theme chain
     *         syncFromSettings();           // pushes values to SFML shapes
     *     }
     * };
     * @endcode
     *
     * ### Locking a component from theme changes
     * @code
     * toggle.lockTheme();    // ignores all future theme changes
     * toggle.unlockTheme();  // resumes reacting to theme changes
     * @endcode
     *
     * ### Locking a component from settings changes
     * @code
     * toggle.lockSettings();    // applySettings() has no effect
     * toggle.unlockSettings();  // applySettings() works again
     * @endcode
     *
     * Explicit calls to individual setters (e.g. @c setTrackOnColor()) always
     * take effect regardless of lock state — locks only block automatic
     * updates from @c ThemeManager and @c applySettings().
     *
     * @see ThemeManager, Theme, Trait
     */
    class Themeable : public Trait
    {
    public:
        // ── Construction / destruction ────────────────────────────────────────

        Themeable()
        {
            ThemeManager::subscribe(this);
        }

        virtual ~Themeable()
        {
            ThemeManager::unsubscribe(this);
        }

        // ── Theme lock ────────────────────────────────────────────────────────

        /**
         * @brief Prevent automatic re-styling when the global theme changes.
         *
         * The component keeps its current visual state. Explicit individual
         * setter calls still work normally.
         */
        void lockTheme()   { _themeLocked = true; }

        /**
         * @brief Resume reacting to global theme changes.
         *
         * Does NOT immediately re-apply the current theme — call
         * @c ThemeManager::get() and pass it to @c onThemeApplied() manually
         * if you want to re-sync immediately after unlocking.
         */
        void unlockTheme() { _themeLocked = false; }

        /** @brief Return @c true if this component ignores theme changes. */
        [[nodiscard]] bool isThemeLocked() const { return _themeLocked; }

        // ── Settings lock ─────────────────────────────────────────────────────

        /**
         * @brief Prevent @c applySettings() from taking effect.
         *
         * Explicit individual setter calls still work normally.
         * Only blocks the batch @c applySettings() path.
         */
        void lockSettings()   { _settingsLocked = true; }

        /**
         * @brief Allow @c applySettings() to take effect again.
         */
        void unlockSettings() { _settingsLocked = false; }

        /** @brief Return @c true if @c applySettings() is currently blocked. */
        [[nodiscard]] bool isSettingsLocked() const { return _settingsLocked; }

        // ── Theme change handler ──────────────────────────────────────────────

        /**
         * @brief Called by @c ThemeManager when the active theme changes.
         *
         * Override in your component to re-style from the new theme.
         * Always check @c isThemeLocked() first:
         *
         * @code
         * void onThemeApplied(const Theme& theme) override
         * {
         *     if (isThemeLocked()) return;
         *     MySettings::applyTheme(theme);
         *     syncFromSettings();
         * }
         * @endcode
         *
         * @param theme  The newly active theme.
         */
        virtual void onThemeApplied(const Theme& theme) = 0;

    private:
        bool _themeLocked    = false;
        bool _settingsLocked = false;
    };

} // namespace ml

#endif // THEMEABLE_H