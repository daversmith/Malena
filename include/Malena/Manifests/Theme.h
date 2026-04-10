//
// Theme.h
//

#ifndef MALENA_THEME_H
#define MALENA_THEME_H

#pragma once

// #include <Malena/Resources/FontManager.h>
#include <Malena/Core/malena_export.h>
#include <SFML/Graphics/Color.hpp>
#include <Malena/Manifests/ThemeTag.h>
#include <SFML/Graphics/Font.hpp>
namespace ml
{
    // ── Theme ─────────────────────────────────────────────────────────────────

    /**
     * @brief Universal design token set applied across all @c Themeable components.
     * @ingroup Manifests
     *
     * A @c Theme is a plain struct of visual tokens — colors, typography, and
     * spacing — that form the visual identity of a UI. Components that inherit
     * @c Themeable automatically re-style themselves when the active theme changes
     * via @c ThemeManager::apply().
     *
     * Malena ships two built-in themes (@c DarkTheme and @c LightTheme).
     * Custom themes are created by inheriting @c Theme and overriding tokens:
     *
     * @code
     * struct NeonTheme : ml::Theme
     * {
     *     NeonTheme()
     *     {
     *         primary    = sf::Color(0,   255, 180);
     *         surface    = sf::Color(10,  10,  20);
     *         background = sf::Color(5,   5,   15);
     *         onSurface  = sf::Color(220, 255, 245);
     *         border     = sf::Color(0,   180, 130);
     *         radius     = 4.f;
     *     }
     * };
     * @endcode
     *
     * ### Registering in a Manifest
     * @code
     * class GameManifest : public ml::Manifest
     * {
     * public:
     *     enum class Themes { Home, GameOver, Menu };
     *
     *     inline static const auto _ = [](){
     *         set(Themes::Home,     ml::DarkTheme());
     *         set(Themes::GameOver, NeonTheme());
     *         set(Themes::Menu,     ml::LightTheme());
     *         return 0;
     *     }();
     * };
     * @endcode
     *
     * ### Applying at runtime
     * @code
     * ml::ThemeManager::apply<GameManifest>(GameManifest::Themes::Home);
     * @endcode
     *
     * @see ThemeManager, Themeable, DarkTheme, LightTheme
     */
    struct Theme : ThemeTag
    {
        // ── Color roles ───────────────────────────────────────────────────────

        /** Primary brand color — buttons, active states, highlights. */
        sf::Color primary    = sf::Color(100, 60,  200);

        /** Secondary accent color — hover states, secondary actions. */
        sf::Color secondary  = sf::Color(70,  130, 230);

        /** Surface color — card backgrounds, input backgrounds. */
        sf::Color surface    = sf::Color(40,  40,  40);

        /** Page/scene background color. */
        sf::Color background = sf::Color(20,  20,  20);

        /** Text/icon color on top of @c surface. */
        sf::Color onSurface  = sf::Color::White;

        /** Text/icon color on top of @c primary. */
        sf::Color onPrimary  = sf::Color::White;

        /** Text/icon color on top of @c background. */
        sf::Color onBackground = sf::Color(220, 220, 220);

        /** Muted text — placeholders, disabled labels, descriptions. */
        sf::Color muted      = sf::Color(120, 120, 120);

        /** Border/outline color for inactive components. */
        sf::Color border     = sf::Color(100, 100, 100);

        /** Border/outline color for focused components. */
        sf::Color borderFocus= sf::Color(100, 60,  200);

        /** Error/invalid state color. */
        sf::Color error      = sf::Color(220, 70,  70);

        /** Success/confirm state color. */
        sf::Color success    = sf::Color(70,  200, 100);

        /** Disabled component color. */
        sf::Color disabled   = sf::Color(60,  60,  60);

        /** Disabled text/icon color. */
        sf::Color onDisabled = sf::Color(120, 120, 120);

        // ── Typography ────────────────────────────────────────────────────────

        /** Default font used by all text-bearing components. */
        const sf::Font* font         = nullptr;

        /** Base font size in points. */
        unsigned int    fontSize     = 14;

        /** Small font size — descriptions, captions. */
        unsigned int    fontSizeSmall= 11;

        /** Large font size — headings, titles. */
        unsigned int    fontSizeLarge= 18;

        // ── Geometry ─────────────────────────────────────────────────────────

        /** Default corner radius for rounded rectangles and buttons. */
        float radius  = 8.f;

        /** Base spacing unit — padding, gaps between elements. */
        float spacing = 8.f;

        /** Default border/outline thickness. */
        float borderThickness = 1.5f;

        // ── Virtual destructor for safe inheritance ───────────────────────────
        virtual ~Theme() = default;
    };

    // ── Built-in themes ───────────────────────────────────────────────────────

    /**
     * @brief Malena default dark theme.
     *
     * Deep charcoal backgrounds with purple primary accents.
     * Suitable for game UIs and dark-mode applications.
     *
     * @see Theme, LightTheme
     */
    struct DarkTheme : Theme
    {
        DarkTheme()
        {
            primary      = sf::Color(100, 60,  200);
            secondary    = sf::Color(70,  130, 230);
            surface      = sf::Color(40,  40,  40);
            background   = sf::Color(20,  20,  20);
            onSurface    = sf::Color::White;
            onPrimary    = sf::Color::White;
            onBackground = sf::Color(220, 220, 220);
            muted        = sf::Color(120, 120, 120);
            border       = sf::Color(100, 100, 100);
            borderFocus  = sf::Color(100, 60,  200);
            error        = sf::Color(220, 70,  70);
            success      = sf::Color(70,  200, 100);
            disabled     = sf::Color(60,  60,  60);
            onDisabled   = sf::Color(120, 120, 120);
            radius       = 8.f;
            spacing      = 8.f;
            borderThickness = 1.5f;
        }
    };

    /**
     * @brief Light theme with clean white surfaces and purple accents.
     *
     * @see Theme, DarkTheme
     */
    struct LightTheme : Theme
    {
        LightTheme()
        {
            primary      = sf::Color(100, 60,  200);
            secondary    = sf::Color(70,  130, 230);
            surface      = sf::Color(245, 245, 248);
            background   = sf::Color(255, 255, 255);
            onSurface    = sf::Color(20,  20,  20);
            onPrimary    = sf::Color::White;
            onBackground = sf::Color(40,  40,  40);
            muted        = sf::Color(150, 150, 150);
            border       = sf::Color(200, 200, 200);
            borderFocus  = sf::Color(100, 60,  200);
            error        = sf::Color(200, 50,  50);
            success      = sf::Color(50,  180, 80);
            disabled     = sf::Color(220, 220, 220);
            onDisabled   = sf::Color(160, 160, 160);
            radius       = 8.f;
            spacing      = 8.f;
            borderThickness = 1.f;
        }
    };

} // namespace ml

#endif // MALENA_THEME_H
