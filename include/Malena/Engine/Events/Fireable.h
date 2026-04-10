// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 3/20/26.
//

#ifndef MALENA_FIREABLE_H
#define MALENA_FIREABLE_H

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Export.h>
#include <functional>
#include <optional>
#include <unordered_map>
#include "SFML/Window/Event.hpp"

#include "Malena/Engine/Events/EventReceiver.h"
#include <Malena/Engine/Events/Callback.h>
#include <Malena/Utilities/EnumKey.h>

namespace ml
{
    namespace exports
    {
        struct FireableRegistrar;
    }

    /// @cond INTERNAL
    /**
     * @brief Lightweight tag inherited by ml::Plugin.
     *
     * Used by FireableHelper to exclude Plugin types from Fireable singleton
     * registration without needing the full Plugin definition.
     */
    struct PluginTag {};
    /// @endcond

    /**
     * @brief Base class for all event dispatchers in the Malena event system.
     * @ingroup EngineEvents
     *
     * @c Fireable is the foundation of the dispatch pipeline. Concrete
     * dispatcher subclasses (e.g. @c ClickableDispatcher, @c KeyableDispatcher)
     * inherit from @c Fireable and are registered automatically at startup via
     * @c ML_EXPORT. Each frame, @c AppManager iterates all registered @c Fireable
     * instances and calls @c fire() for every matching SFML event.
     *
     * User code does not instantiate or interact with @c Fireable directly.
     * Use the trait callback methods (@c onClick, @c onHover, @c onUpdate, etc.)
     * on your components instead.
     *
     * @see EventDispatcher, FrameDispatcher, ML_EXPORT, AppManager
     */
    class Core;
    class EventReceiver;
    class MALENA_API Fireable
    {
    public:
        /// @cond INTERNAL
        enum class DispatchType { FRAME, EVENT };
        Fireable(DispatchType type = DispatchType::EVENT);
        /// @endcond

        virtual ~Fireable() = default;

        /// @cond INTERNAL
        virtual bool occurred(const std::optional<sf::Event>& event) { return false; }
        virtual bool occurred() { return false; }
        virtual bool filter(const std::optional<sf::Event>& event, Core* component) { return true; }

        template<typename EnumType>
        static void addCallback(EnumType eventEnum,
                                EventReceiver* component,
                                EventCallback callback);

        struct MALENA_API Cleanup { ~Cleanup(); };

        static void _register(Fireable* f);
        static void _unregister(Fireable* f);

        inline static std::vector<Fireable*> _fireables;
        inline static std::vector<Fireable*> _fireables_frame;
        inline static Cleanup               _cleanup;

        virtual void fire(const std::optional<sf::Event>& event) = 0;
        virtual void fire() = 0;
        /// @endcond

        friend class AppManager;
        friend class PluginManager;
        friend struct ml::exports::FireableRegistrar;
        template<typename T> friend struct MLExport;

    private:
        /// @cond INTERNAL
        DispatchType _type;
        /// @endcond
    };

} // namespace ml

// ── FireableRegistrar ─────────────────────────────────────────────────────────

/// @cond INTERNAL

namespace ml::exports
{
    struct FireableRegistrar
    {
        static void registerFireable(ml::Fireable* f)
        {
            ml::Fireable::_register(f);
        }
    };
}

namespace ml::exports
{
    template<typename T, typename = void>
    struct FireableHelper
    {
        static void doRegister() {}
    };

    template<typename T>
    struct FireableHelper<T, std::enable_if_t<
        std::is_base_of_v<ml::Fireable, T> &&
        !std::is_base_of_v<ml::PluginTag, T>
    >>
    {
        static void doRegister()
        {
            static bool registered = false;
            if (registered) return;
            registered = true;
            static T dispatcher;
            ml::exports::FireableRegistrar::registerFireable(&dispatcher);
        }
    };
}

/// @endcond

#include "../../../../src/Engine/Events/Fireable.tpp"
#endif // MALENA_FIREABLE_H
