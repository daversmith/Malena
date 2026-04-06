//
// Created by Dave Smith on 12/7/25.
//
#include <Malena/Traits/Communication/Messenger.h>
namespace ml
{
    void Messenger::offAllMessages() {
        MessageManager::unsubscribeAll(this);
    }

    Messenger::~Messenger() {
        // Auto-cleanup when object is destroyed
        offAllMessages();
    }
}