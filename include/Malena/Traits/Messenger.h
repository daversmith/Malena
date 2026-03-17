#ifndef MALENA_MESSENGER_NEW_H
#define MALENA_MESSENGER_NEW_H

#include <Malena/Engine/Messaging/MessageManager.h>
#include <functional>
#include "Base/Trait.h"

namespace ml
{
    /**
     * @brief Trait for sending and receiving typed, enum-keyed messages.
     *
     * @c Messenger wraps @c MessageManager with a clean per-object API for
     * structured communication between framework objects and plugins.
     * It is distinct from @c Subscribable, which handles string-keyed
     * input events fired by @c UIManager.
     *
     * Where @c Subscribable is for reacting to @e input (@c "click",
     * @c "hover", @c "update"), @c Messenger is for reacting to
     * @e application-level signals — for example, a loading plugin telling
     * the launcher it has finished, or a game plugin broadcasting a score
     * change. Messages are keyed by a user-defined enum and carry a
     * strongly-typed payload.
     *
     * The destructor calls @c offAllMessages() automatically, so subscriptions
     * are always cleaned up when the owning object is destroyed.
     *
     * ### Usage
     * @code
     * enum class GameEvent { Started, ScoreChanged, Stopped };
     *
     * // Subscribe
     * onMessage<int>(GameEvent::ScoreChanged, [](const int& score) {
     *     updateScoreDisplay(score);
     * });
     *
     * // Publish
     * sendMessage<int>(GameEvent::ScoreChanged, 42);
     *
     * // Unsubscribe from one event
     * offMessage<int>(GameEvent::ScoreChanged);
     *
     * // Unsubscribe from everything
     * offAllMessages();
     * @endcode
     *
     * @see MessageManager, Subscribable, Plugin
     */
    class Messenger : public Trait
    {
    public:
        /**
         * @brief Publish a typed message to all current subscribers.
         *
         * All objects that have called @c onMessage for the same
         * @p event and @c DataType will have their callbacks invoked
         * with @p data. Delivery is immediate unless @c MessageManager
         * is currently iterating, in which case it is deferred.
         *
         * @tparam DataType The payload type. Must match the type used
         *                  in the corresponding @c onMessage call.
         * @tparam Enum     The enum type identifying the event.
         * @param  event    The specific event to send.
         * @param  data     The payload to deliver to subscribers.
         */
        template<typename DataType, typename Enum>
        void sendMessage(Enum event, const DataType& data);

        /**
         * @brief Register a callback to receive a typed message.
         *
         * The callback is invoked each time @c sendMessage is called
         * with the same @p event and @c DataType. Multiple calls with
         * the same @p event replace the previous callback for that event.
         *
         * @tparam DataType  The expected payload type.
         * @tparam Enum      The enum type identifying the event.
         * @param  event     The specific event to listen for.
         * @param  callback  Function called with a const reference to
         *                   the payload when the event fires.
         */
        template<typename DataType, typename Enum>
        void onMessage(Enum event, std::function<void(const DataType&)> callback);

        /**
         * @brief Unsubscribe from a specific typed message.
         *
         * After this call, the callback previously registered for
         * @p event and @c DataType will no longer be invoked. Safe to
         * call from within the callback itself.
         *
         * @tparam DataType The payload type of the subscription to remove.
         * @tparam Enum     The enum type identifying the event.
         * @param  event    The specific event to unsubscribe from.
         */
        template<typename DataType, typename Enum>
        void offMessage(Enum event);

        /**
         * @brief Unsubscribe from all messages registered by this object.
         *
         * Called automatically by the destructor. Only call manually if
         * you need to reset all subscriptions while the object is still alive.
         */
        void offAllMessages();

        virtual ~Messenger();
        Messenger() = default;
    };

} // namespace ml

#include "../../../src/Traits/Messenger.tpp"
#endif // MALENA_MESSENGER_NEW_H
