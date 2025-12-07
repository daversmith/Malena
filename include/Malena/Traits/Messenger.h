#ifndef MALENA_MESSENGER_NEW_H
#define MALENA_MESSENGER_NEW_H

#include <Malena/Events/EventBus.h>
#include <functional>

namespace ml {

  /**
   * Messenger - Trait for sending/receiving type-safe messages
   * Wraps EventBus static service similar to how old Messenger wraps EventsManager
   */
  class Messenger {
  public:

    /**
     * Send a message to all listeners
     * @tparam DataType - The data type being sent
     * @tparam Enum - The event enum type
     * @param event - The specific event
     * @param data - The message data
     */
    template<typename DataType, typename Enum>
    void sendMessage(Enum event, const DataType& data);
    /**
     * Listen for a specific message
     * @tparam DataType - The data type to receive
     * @tparam Enum - The event enum type
     * @param event - The specific event to listen for
     * @param callback - Function to call when message received
     */
    template<typename DataType, typename Enum>
    void onMessage(Enum event, std::function<void(const DataType&)> callback);

    /**
     * Stop listening to a specific message
     * @tparam DataType - The data type
     * @tparam Enum - The event enum type
     * @param event - The specific event to unsubscribe from
     */
    template<typename DataType, typename Enum>
    void offMessage(Enum event);

    /**
     * Stop listening to all messages
     */
    void offAllMessages();

    virtual ~Messenger();
    Messenger()= default;
  };

} // namespace ml
#include "../../../src/Traits/Messenger.tpp"
#endif // MALENA_MESSENGER_NEW_H