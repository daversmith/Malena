// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Application.h
//

#ifndef APPLICATION_H
#define APPLICATION_H

#pragma once

#include <Malena/Core/CoreManager.h>
#include <Malena/Engine/App/AppManager.h>
#include <Malena/Engine/App/UIController.h>
#include <Malena/Manifests/Manifest.h>
#include <string>
#include <type_traits>

#include <Malena/Manifests/ManifestResources.h>

namespace ml
{
    // ── ApplicationBase — internal base shared by Application and ApplicationWith ──

    /**
     * @brief Internal base class for all Malena applications.
     * @ingroup EngineApp
     *
     * Do not inherit directly. Use @c ml::Application or @c ml::ApplicationWith.
     *
     * @see Application, ApplicationWith
     */
    class ApplicationBase : public AppManager, public UIController
    {
    public:
        /**
         * @brief Construct with a separate @c UIController and explicit video mode.
         */
        ApplicationBase(const sf::VideoMode& videoMode,
                        const std::string& title,
                        UIController& appLogic,
                        sf::RenderWindow& window = WindowManager::getWindow());

        /**
         * @brief Construct from pixel dimensions and bit depth.
         */
        ApplicationBase(unsigned int screenWidth,
                        unsigned int screenHeight,
                        unsigned int bitDepth,
                        const std::string& title);

        /**
         * @brief Construct from an SFML video mode, using @c *this as the controller.
         */
        ApplicationBase(const sf::VideoMode& videoMode,
                        const std::string& title);

        /**
         * @brief Register a @c Core object with the application's component manager.
         */
        void addComponent(Core& component);

        /**
         * @brief Clear all event subscriptions across the entire application.
         */
        static void clearEvents();

        /**
         * @brief Full application reset — clears events, messages, and components.
         *
         * Tears down all framework state in the correct order:
         * 1. All event subscriptions
         * 2. All message subscriptions
         * 3. All registered components
         *
         * After @c reset(), call @c initialization() and @c registerEvents()
         * to rebuild the scene.
         *
         * @note Does NOT close the window or restart the main loop.
         */
        static void reset();
    };

    // ── Application — no manifest ─────────────────────────────────────────────

    /**
     * @brief Primary entry point for Malena applications without a manifest.
     * @ingroup EngineApp
     *
     * Combines @c AppManager and @c UIController into a single class.
     * Override @c initialization() and @c registerEvents() to build your scene.
     *
     * @code
     * class MyApp : public ml::Application
     * {
     * public:
     *     MyApp() : ml::Application(1280, 720, 32, "My App") {}
     *
     *     void initialization() override
     *     {
     *         _box.setSize({200.f, 100.f});
     *         addComponent(_box);
     *     }
     *
     *     void registerEvents() override
     *     {
     *         _box.onClick([]{ std::cout << "clicked!\n"; });
     *     }
     *
     * private:
     *     ml::Rectangle _box;
     * };
     * @endcode
     *
     * @see ApplicationWith, AppManager, UIController
     */
    class Application : public ApplicationBase
    {
    public:
        using ApplicationBase::ApplicationBase;

        void initialization() override = 0;
        void registerEvents() override = 0;
    };

    // ── ApplicationWith — with manifest ───────────────────────────────────────

    /**
     * @brief Entry point for Malena applications with a manifest.
     * @ingroup EngineApp
     *
     * Extends @c Application with manifest support — injects @c Resources::get()
     * and all manifest enum aliases into scope automatically. Use when your
     * application has global resources (icon, main font, app-wide config).
     *
     * @code
     * class MyManifest : public ml::Manifest
     * {
     * public:
     *     static constexpr const char* name    = "My App";
     *     static constexpr const char* version = "1.0.0";
     *
     *     enum class Images { Icon };
     *     enum class Fonts  { Main };
     *     enum class Text   { WindowTitle };
     *     enum class Ints   { TargetFPS };
     *
     * private:
     *     inline static const auto _ = [](){
     *         set(Images::Icon,       "assets/icon.png");
     *         set(Fonts::Main,        "assets/main.ttf");
     *         set(Text::WindowTitle,  std::string("My App"));
     *         set(Ints::TargetFPS,    60);
     *         return 0;
     *     }();
     * };
     *
     * class MyApp : public ml::ApplicationWith<MyManifest>
     * {
     * public:
     *     MyApp() : ml::ApplicationWith<MyManifest>(
     *         1280, 720, 32,
     *         Resources::get(Text::WindowTitle)) {}
     *
     *     void initialization() override
     *     {
     *         auto& icon = Resources::get(Images::Icon);
     *         auto& font = Resources::get(Fonts::Main);
     *         int   fps  = Resources::get(Ints::TargetFPS);
     *     }
     *
     *     void registerEvents() override { ... }
     * };
     * @endcode
     *
     * @tparam Manifest A @c Manifest subclass declaring app-wide resources
     *                  and configuration.
     *
     * @see Application, ApplicationBase, Resources
     */
    template<typename Manifest>
    class ApplicationWith : public ApplicationBase,
                             public ml::ManifestResources<Manifest>
    {
    public:
        /// Unified resource accessor — Resources::get(Images::Icon)
        using Resources = ml::ManifestResources<Manifest>;

        using ApplicationBase::ApplicationBase;

        void initialization() override = 0;
        void registerEvents() override = 0;
    };

} // namespace ml

#endif // APPLICATION_H