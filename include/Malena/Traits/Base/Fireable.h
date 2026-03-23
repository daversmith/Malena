//
// Created by Dave Smith on 3/20/26.
//

#ifndef FIREABLE_H
#define FIREABLE_H

#include <Malena/Core/Export.h>
#include <functional>
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

    /**
     * @brief Lightweight tag inherited by ml::Plugin.
     * Used by FireableHelper to exclude Plugin types from
     * Fireable singleton registration without needing the
     * full Plugin definition.
     * @cond INTERNAL
     */
    struct PluginTag {};   // ← here, before Fireable

    /**
     * @brief Base class for all event dispatchers in the Malena event system.
     * @ingroup Traits
     */
	class Core;
	class EventReceiver;
    class Fireable
    {
    public:
        enum class DispatchType {FRAME, EVENT};
        Fireable(DispatchType type = DispatchType::EVENT);
        virtual ~Fireable() = default;

    // protected:
        virtual bool occurred(const std::optional<sf::Event>& event) {return false;};
        virtual bool occurred() {return false;};
        virtual bool filter(const std::optional<sf::Event> &event, Core *component)
		{return true;};


        // static void addCallback(const std::string &eventName, EventReceiver *component, Callback callback);
    	template<typename EnumType>
				static void addCallback(EnumType eventEnum,
										 EventReceiver* component,
										 EventCallback callback);


        struct Cleanup { ~Cleanup(); };

        static void _register(Fireable* f);
        static void _unregister(Fireable* f);

        inline static std::vector<Fireable*> _fireables;
        inline static std::vector<Fireable*> _fireables_frame;
        inline static Cleanup               _cleanup;

        virtual void fire(const std::optional<sf::Event>& event) = 0;
        virtual void fire() = 0;
        friend class AppManager;
        friend class PluginManager;
        friend struct ml::exports::FireableRegistrar;
        template<typename T> friend struct MLExport;
    private:
        DispatchType _type;
    };

} // namespace ml

// ── FireableRegistrar ─────────────────────────────────────────────────────────

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

// ── FireableHelper ────────────────────────────────────────────────────────────
//
//  PRIMARY TEMPLATE ONLY — no-op for all types.
//
//  The specialization that actually registers Fireable types lives in
//  Plugin.h — AFTER ml::Plugin is fully defined — so that
//  !std::is_base_of_v<ml::Plugin, T> can be correctly evaluated.
//
//  If FireableHelper is used before Plugin.h is included (e.g. in
//  Fireable-only contexts), the primary no-op fires and nothing is
//  registered. Plugin.h redefines ML_EXPORT which includes the full
//  specialization, so any ML_EXPORT call after Plugin.h is included
//  gets the correct behaviour.
//
// ─────────────────────────────────────────────────────────────────────────────

namespace ml::exports
{
    /**
     * @brief Primary template — no-op for all types.
     *        Full specialization lives in Plugin.h where Plugin is complete.
     * @cond INTERNAL
     */
    template<typename T, typename = void>
    struct FireableHelper
    {
        static void doRegister() {}
    };
    /** @endcond */
}
namespace ml::exports
{
    /**
     * @brief Specialisation for Fireable-only types.
     *        Plugin types and Plugin+Fireable types are excluded —
     *        PluginManager handles their registration.
     * @cond INTERNAL
     */
    template<typename T>
    struct FireableHelper<T, std::enable_if_t<
        std::is_base_of_v<ml::Fireable, T> &&
        !std::is_base_of_v<ml::PluginTag,  T>   // Plugin complete here — correct evaluation
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
    /** @endcond */
}
#include "../../../../src/Traits/Base/Fireable.tpp"
#endif // FIREABLE_H
