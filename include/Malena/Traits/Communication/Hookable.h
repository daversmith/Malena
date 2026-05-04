// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

#ifndef MALENA_HOOKABLE_H
#define MALENA_HOOKABLE_H

#include <Malena/Core/malena_export.h>
#include <Malena/Traits/Communication/Messenger.h>
#include <memory>
#include <type_traits>

namespace ml
{
    namespace detail
    {
        template<typename F> struct cb_arg;
        template<typename C, typename R, typename A>
        struct cb_arg<R(C::*)(A) const> { using type = std::decay_t<A>; };
        template<typename C, typename R, typename A>
        struct cb_arg<R(C::*)(A)>       { using type = std::decay_t<A>; };
    }

    template<typename F>
    using cb_arg_t = typename detail::cb_arg<decltype(&std::decay_t<F>::operator())>::type;

    /**
     * @brief Trait that layers a hook system on top of ml::Messenger.
     *
     * Inherit Hookable to gain sendHook / onHook / sharedAs on any class
     * that participates in the message bus.
     *
     * - sendHook(enum)           — broadcast this object as a shared hook payload.
     * - onHook(enum, callback)   — subscribe; callback receives T& or derived T&
     *                              (dynamic_cast applied automatically).
     * - sharedAs<T>()            — get a shared_ptr<T> to this (for lifetime capture).
     *
     * Classes that call sendHook must be managed by a shared_ptr.
     * Classes that only call onHook (e.g. AppShell) do not need one.
     *
     * @ingroup Traits
     */
    struct MALENA_API Hookable : public ml::Messenger
    {
        virtual ~Hookable() = default;

        template<typename Enum>
        void sendHook(Enum hook) {
            sendMessage<Hookable*>(hook, this);
        }

        template<typename Enum, typename Callback>
        void onHook(Enum hook, Callback&& cb) {
            using T = cb_arg_t<Callback>;
            onMessage<Hookable*>(hook,
                [cb = std::forward<Callback>(cb)](Hookable* w) {
                    if constexpr (std::is_same_v<T, Hookable>) {
                        cb(*w);
                    } else {
                        if (auto* p = dynamic_cast<T*>(w))
                            cb(*p);
                    }
                }
            );
        }
    };

} // namespace ml

#endif // MALENA_HOOKABLE_H
