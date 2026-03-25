// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// MessageManager.h
//

#ifndef MALENA_EVENTBUS_H
#define MALENA_EVENTBUS_H

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <algorithm>
#include <optional>
#include <Malena/Core/DeferredOperationsManager.h>

namespace ml
{
    /**
     * @brief Typed, enum-keyed message bus for structured inter-object communication.
     * @ingroup EngineMessaging
     *
     * @c MessageManager is the engine behind the @c Messenger trait. It
     * provides a completely separate communication channel from @c EventsManager:
     *
     * | @c EventsManager | @c MessageManager |
     * |------------------|-------------------|
     * | String keys (@c "click", @c "hover") | Enum keys (@c GameEvent::Started) |
     * | Fired by dispatchers based on SFML input | Fired explicitly by application code |
     * | Delivers to @c Subscribable objects | Delivers to any @c void* subscriber |
     * | Used for input and UI lifecycle events | Used for application-level signals |
     *
     * ### How the key works
     * Each subscription is identified by a three-part key:
     * - The enum type (@c std::type_index of @c Enum)
     * - The enum value (cast to @c int)
     * - The data type (@c std::type_index of @c DataType)
     *
     * This means the same enum value can carry different payload types without
     * collision, and different enum types are always distinct even if their
     * integer values overlap.
     *
     * ### Iteration safety
     * @c MessageManager inherits @c DeferredOperationsManager<MessageManager>.
     * Any @c unsubscribe or @c unsubscribeAll call made while @c publish() is
     * iterating is automatically deferred and processed after the iteration
     * completes. This makes it safe to unsubscribe from within a message
     * callback.
     *
     * ### Usage
     * Most user code interacts with @c MessageManager through the @c Messenger
     * trait methods (@c sendMessage, @c onMessage, @c offMessage). Direct calls
     * are only needed for framework-internal wiring.
     *
     * @code
     * enum class GameEvent { Started, ScoreChanged, Stopped };
     *
     * // Subscribe (via Messenger trait — preferred)
     * onMessage<int>(GameEvent::ScoreChanged, [](const int& score) {
     *     updateHUD(score);
     * });
     *
     * // Publish (via Messenger trait — preferred)
     * sendMessage<int>(GameEvent::ScoreChanged, 42);
     * @endcode
     *
     * @see Messenger, EventsManager, DeferredOperationsManager
     */
    class MessageManager : public DeferredOperationsManager<MessageManager>
    {
        /// @cond INTERNAL
        using EventKey = std::tuple<std::type_index, int, std::type_index>;

        struct KeyHash
        {
            std::size_t operator()(const EventKey& key) const
            {
                auto h1 = std::get<0>(key).hash_code();
                auto h2 = std::hash<int>{}(std::get<1>(key));
                auto h3 = std::get<2>(key).hash_code();
                return h1 ^ (h2 << 1) ^ (h3 << 2);
            }
        };

        struct Subscription
        {
            void* subscriber;
            std::function<void(const void*)> callback;
        };

        static std::unordered_map<EventKey, std::vector<Subscription>, KeyHash> subscribers;

        static void doUnsubscribe(const EventKey& key, void* subscriber);
        static void doUnsubscribeAll(void* subscriber);
        /// @endcond

    public:
        /**
         * @brief Register a typed callback for a specific enum event.
         *
         * The callback is stored against the composite key formed by
         * @c Enum, @p event, and @c DataType. If @p subscriber has already
         * registered a callback for this exact key, the new callback replaces
         * the old one.
         *
         * @tparam DataType   Expected payload type.
         * @tparam Enum       Enum type identifying the message.
         * @param  event      The specific enum value to subscribe to.
         * @param  subscriber Opaque pointer identifying the subscribing object
         *                    (typically @c this). Used for targeted unsubscription.
         * @param  callback   Called with a const reference to the payload when
         *                    the message is published.
         */
        template<typename DataType, typename Enum>
        static void subscribe(Enum event, void* subscriber,
                              std::function<void(const DataType&)> callback);

        /**
         * @brief Deliver a typed message to all matching subscribers.
         *
         * Looks up all subscriptions matching the composite key for @p event
         * and @c DataType, then invokes each callback with @p data. The
         * manager marks itself busy for the duration; any unsubscriptions
         * that arrive during delivery are deferred.
         *
         * @tparam DataType  Payload type. Must match the type used in the
         *                   corresponding @c subscribe call.
         * @tparam Enum      Enum type identifying the message.
         * @param  event     The specific enum value to publish.
         * @param  data      Payload delivered to all matching callbacks.
         */
        template<typename DataType, typename Enum>
        static void publish(Enum event, const DataType& data);

        /**
         * @brief Remove one subscriber's callback for a specific typed event.
         *
         * If called during a @c publish() iteration the removal is deferred.
         *
         * @tparam DataType  Payload type of the subscription to remove.
         * @tparam Enum      Enum type identifying the message.
         * @param  event     The specific enum value to unsubscribe from.
         * @param  subscriber The subscribing object to remove.
         */
        template<typename DataType, typename Enum>
        static void unsubscribe(Enum event, void* subscriber);

        /**
         * @brief Remove all subscriptions for a given subscriber.
         *
         * Called automatically by @c Messenger's destructor. If called
         * during a @c publish() iteration the removal is deferred, preventing
         * use-after-free when an object destroys itself inside a callback.
         *
         * @param subscriber The subscribing object to remove from all messages.
         */
        static void unsubscribeAll(void* subscriber);

        /**
         * @brief Remove all subscriptions for all subscribers and all keys.
         *
         * Use during application teardown or full scene reset.
         */
        static void clear();

        /// @cond INTERNAL
        /**
         * @brief Unconditionally remove all subscriptions for @p subscriber.
         *
         * Bypasses the deferred-operation mechanism. Only safe to call
         * from @c PluginManager during plugin unloading, where the plugin's
         * memory is about to be freed and a deferred removal would leave a
         * dangling pointer.
         *
         * @warning Do not call from user code or inside a @c publish() callback.
         */
        static void forceUnsubscribeAll(void* subscriber);
        /// @endcond
    };

} // namespace ml

#include "../../../../src/Engine/Messaging/MessageManager.tpp"
#endif // MALENA_EVENTBUS_H
