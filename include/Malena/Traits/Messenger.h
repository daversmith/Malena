#ifndef MALENA_MESSENGER_NEW_H
#define MALENA_MESSENGER_NEW_H

#include <Malena/Engine/Messaging/MessageManager.h>
#include <functional>
#include <Malena/Traits/Base/Trait.h>

namespace ml
{
    /**
     * @brief Trait that adds typed, enum-keyed message sending and receiving
     *        to any class.
     * @ingroup Traits
     *
     * @c Messenger is a self-contained trait — it has no dependency on
     * @c Subscribable, @c Core, or any other framework base class. Any class
     * can gain message-bus participation simply by inheriting it:
     *
     * - @c ml::Plugin inherits @c Messenger directly, so every plugin is a
     *   first-class message-bus participant out of the box.
     * - Components gain it by mixing it in:
     *   @c ComponentWith<MyManifest, ml::Messenger>.
     * - Plain application objects can inherit @c Messenger directly.
     *
     * ### What it provides
     * A thin, per-object wrapper over @c MessageManager. Each @c Messenger
     * instance tracks which messages @e it has subscribed to, so that
     * @c offAllMessages() (called automatically by the destructor) only
     * removes @e this object's subscriptions — never anyone else's.
     *
     * Messages are keyed by a user-defined @c enum @c class value and carry
     * a strongly-typed payload. The sender and receiver only need to share the
     * enum header — neither needs to know about the other's class.
     *
     * ### Usage
     * @code
     * enum class GameEvent { Started, ScoreChanged, Stopped };
     *
     * // Subscribe — callback receives a const T& payload
     * onMessage<int>(GameEvent::ScoreChanged, [](const int& score) {
     *     updateScoreDisplay(score);
     * });
     *
     * // Send — all subscribers for this key are notified immediately
     * sendMessage<int>(GameEvent::ScoreChanged, 42);
     *
     * // Unsubscribe from one key
     * offMessage<int>(GameEvent::ScoreChanged);
     *
     * // Unsubscribe from everything this object has registered
     * offAllMessages();
     * @endcode
     *
     * ### Lifetime safety
     * The destructor calls @c offAllMessages() automatically. Subscriptions
     * are always cleaned up when the owning object is destroyed, with no
     * manual teardown required. It is also safe to call @c offMessage or
     * @c offAllMessages from inside a callback — removals are deferred by
     * @c MessageManager until the current delivery pass completes.
     *
     * @see MessageManager, Plugin, PluginWith
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