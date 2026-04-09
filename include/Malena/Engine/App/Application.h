// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Application.h
//

#ifndef APPLICATION_H
#define APPLICATION_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Lifecycle/Updatable.h>
#include <Malena/Core/CoreManager.h>
#include <Malena/Engine/App/AppManager.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Manifests/ManifestResources.h>
#include <string>
#include <type_traits>

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
    class MALENA_API ApplicationBase : public AppManager, public Updatable
    {
    public:
        /**
         * @brief Construct with an explicit video mode and window title.
         */
        ApplicationBase(const sf::VideoMode& videoMode,
                        const std::string& title,
                        sf::RenderWindow& window = WindowManager::getWindow());

        /**
         * @brief Construct from pixel dimensions and bit depth.
         */
        ApplicationBase(unsigned int screenWidth,
                        unsigned int screenHeight,
                        unsigned int bitDepth,
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
         * After @c reset(), call @c onInit() and @c onReady() to rebuild
         * the scene.
         *
         * @note Does NOT close the window or restart the main loop.
         */
        void reset();

    	~ApplicationBase();
    };

    // ── Application — no manifest ─────────────────────────────────────────────

    /**
     * @brief Primary entry point for Malena applications without a manifest.
     * @ingroup EngineApp
     *
     * Combines @c AppManager and @c Lifecycle into a single class.
     * Override @c onInit() to set up components and @c onReady() to wire
     * event callbacks.
     *
     * @code
     * class MyApp : public ml::Application
     * {
     * public:
     *     MyApp() : ml::Application(1280, 720, 32, "My App") {}
     *
     * protected:
     *     void onInit() override
     *     {
     *         _box.setSize({200.f, 100.f});
     *         addComponent(_box);
     *     }
     *
     *     void onReady() override
     *     {
     *         _box.onClick([]{ std::cout << "clicked!\n"; });
     *     }
     *
     * private:
     *     ml::Rectangle _box;
     * };
     * @endcode
     *
     * @see ApplicationWith, AppManager
     */
    class MALENA_API Application : public ApplicationBase
    {
    public:
        using ApplicationBase::ApplicationBase;

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
     *     void onInit() override
     *     {
     *         auto& icon = Resources::get(Images::Icon);
     *         auto& font = Resources::get(Fonts::Main);
     *         int   fps  = Resources::get(Ints::TargetFPS);
     *     }
     *
     *     void onReady() override {  }
     * };
     * @endcode
     *
     * @tparam Manifest A @c Manifest subclass declaring app-wide resources
     *                  and configuration.
     *
     * @see Application, ApplicationBase, Resources
     */
    template<typename TManifest>
    class ApplicationWith : public ApplicationBase,
                             public ml::ManifestResources<TManifest>
    {
    public:
        /// Unified resource accessor — Resources::get(Images::Icon)
        using Resources = ml::ManifestResources<TManifest>;

        using ApplicationBase::ApplicationBase;

    };

} // namespace ml

#endif // APPLICATION_H