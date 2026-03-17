#pragma once

/**
 * @defgroup Malena Malena Framework
 * @brief Event-driven C++ framework built on top of SFML.
 *
 * Malena provides application structure, messaging, plugins, reusable graphics
 * components, trait-based behavior composition, resource management, manifests,
 * utilities, and optional ECS support.
 */

/**
 * @defgroup Core Core
 * @brief Foundational framework types, base abstractions, and shared managers.
 * @ingroup Malena
 */

/**
 * @defgroup Engine Engine
 * @brief Runtime systems including application flow, events, messaging, plugins, and window management.
 * @ingroup Malena
 */

/**
 * @defgroup Graphics Graphics
 * @brief Reusable visual components and UI systems built on top of SFML.
 * @ingroup Malena
 */

/**
 * @defgroup ECS ECS
 * @brief Entity-component-system support for data-oriented architecture.
 * @ingroup Malena
 */

/**
 * @defgroup Manifests Manifests
 * @brief Declarative metadata, aliases, resources, and context definitions.
 * @ingroup Malena
 */

/**
 * @defgroup Resources Resources
 * @brief Asset, configuration, font, sound, texture, flag, and state management systems.
 * @ingroup Malena
 */

/**
 * @defgroup Traits Traits
 * @brief Reusable mixin-style behaviors used throughout the framework.
 * @ingroup Malena
 */

/**
 * @defgroup Utilities Utilities
 * @brief Shared helper types and utility functions used across the framework.
 * @ingroup Malena
 */

/* =========================
   Engine Subgroups
   ========================= */

/**
 * @defgroup EngineApp Engine/App
 * @brief Application entry points, managers, and controller abstractions.
 * @ingroup Engine
 */

/**
 * @defgroup EngineEvents Engine/Events
 * @brief Engine-level event management infrastructure.
 * @ingroup Engine
 */

/**
 * @defgroup EngineMessaging Engine/Messaging
 * @brief Messaging infrastructure for event-driven communication.
 * @ingroup Engine
 */

/**
 * @defgroup EnginePlugins Engine/Plugins
 * @brief Plugin interfaces, metadata, and plugin management.
 * @ingroup Engine
 */

/**
 * @defgroup EngineWindow Engine/Window
 * @brief Window ownership and access helpers.
 * @ingroup Engine
 */

/* =========================
   Graphics Subgroups
   ========================= */

/**
 * @defgroup GraphicsBase Graphics/Base
 * @brief Foundational graphic and button abstractions.
 * @ingroup Graphics
 */

/**
 * @defgroup GraphicsControls Graphics/Controls
 * @brief Interactive UI controls such as carousels and button types.
 * @ingroup Graphics
 */

/**
 * @defgroup GraphicsLayouts Graphics/Layouts
 * @brief Layout utilities for arranging visual components.
 * @ingroup Graphics
 */

/**
 * @defgroup GraphicsPrimitives Graphics/Primitives
 * @brief Primitive drawable shapes and low-level visual building blocks.
 * @ingroup Graphics
 */

/**
 * @defgroup GraphicsSprites Graphics/Sprites
 * @brief Sprite-based graphics abstractions.
 * @ingroup Graphics
 */

/**
 * @defgroup GraphicsText Graphics/Text
 * @brief Text rendering, input, cursor, and typing systems.
 * @ingroup Graphics
 */

/* =========================
   ECS Subgroups
   ========================= */

/**
 * @defgroup ECSCore ECS/Core
 * @brief Core ECS data structures and management systems.
 * @ingroup ECS
 */

/**
 * @defgroup ECSSystem ECS/System
 * @brief ECS systems that process components and entities.
 * @ingroup ECS
 */

/* =========================
   Traits Subgroups
   ========================= */

/**
 * @defgroup TraitsBase Traits/Base
 * @brief Base infrastructure for trait composition and multi-trait helpers.
 * @ingroup Traits
 */