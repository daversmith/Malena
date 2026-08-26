// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

//
// Unsubscribable.h
//

#ifndef MALENA_UNSUBSCRIBABLE_H
#define MALENA_UNSUBSCRIBABLE_H

#pragma once


#include <Malena/Core/malena_export.h>
#include <Malena/Utilities/EnumKey.h>
namespace ml
{
    class Core;   // fwd: Unsubscribable stores the Core identity (see _selfCore)
    /**
     * @brief Trait that gives components the ability to unsubscribe from events.
     * @ingroup Traits
     *
     * @c Unsubscribable is a separate trait inherited by @c Core. It provides
     * @c unsubscribe() and @c unsubscribeAll() without touching @c EventReceiver,
     * avoiding the diamond ambiguity that would arise if unsubscribe logic lived
     * in @c EventReceiver (which is multiply inherited through traits).
     *
     * Both built-in @c ml::Event values and user-defined manifest event enums
     * work identically — any enum value is converted to a @c size_t key via
     * @c EnumKey::get().
     *
     * ### Usage
     * @code
     * // Unsubscribe from a built-in event
     * myRect.unsubscribe(ml::Event::CLICK);
     * myRect.unsubscribe(ml::Event::HOVER);
     *
     * // Unsubscribe from a custom manifest event
     * myRect.unsubscribe(MyManifest::Event::SCORE_CHANGED);
     *
     * // Remove all subscriptions
     * myRect.unsubscribeAll();
     * @endcode
     *
     * @note @c unsubscribeAll() is called automatically by @c Core's destructor.
     *       Explicit calls are only needed for mid-lifecycle cleanup.
     *
     * @see Core, EventReceiver, EventManager, EnumKey, ml::Event
     */
    class MALENA_API Unsubscribable
    {
    public:
        /**
         * @brief Unsubscribe from a single event by enum value.
         *
         * Works with any enum type — @c ml::Event for built-in events,
         * or a manifest @c Event enum for custom events.
         *
         * If called during an @c EventManager::fire() iteration the
         * removal is deferred until the iteration completes.
         *
         * @tparam EnumType Any enum type. Enforced at compile time.
         * @param  event    The enum value identifying the event to remove.
         */
    	template<typename EnumType>
		void unsubscribe(EnumType event)
    	{
    		static_assert(std::is_enum_v<EnumType>,
				"[Malena] unsubscribe — argument must be an enum value.");
    		doUnsubscribe(EnumKey::get(event));
    	}


        /**
         * @brief Remove all event subscriptions for this component.
         *
         * Removes every subscription registered across all event names.
         * Called automatically by @c Core's destructor.
         *
         * If called during an @c EventManager::fire() iteration the
         * removal is deferred until the iteration completes.
         */
        void unsubscribeAll();

        virtual ~Unsubscribable() = default;
    private:
    	void doUnsubscribe(const std::string& key);

        // Core's own identity, stamped by Core's constructor.
        //
        // Why not just dynamic_cast<Core*>(this) at the point of use: widgets
        // call unsubscribeAll() from INSIDE their own constructor (ml::Panel
        // does, and several app scenes do) to drop the empty click/hover
        // subscriptions ComponentCore auto-registers. At that moment the object
        // is still under construction, so RTTI on it is undefined — Clang
        // tolerates it, MSVC hard-fails the process (0xC0000409). Core sets this
        // pointer before any derived constructor runs, so it is always valid by
        // then and needs no RTTI at all.
        Core* _selfCore = nullptr;
        friend class Core;
    };

} // namespace ml

#endif // MALENA_UNSUBSCRIBABLE_H
