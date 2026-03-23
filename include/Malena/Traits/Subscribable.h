//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef SUBSCRIBABLE_H
#define SUBSCRIBABLE_H

#pragma once

#include <Malena/Engine/Events/EventsManager.h>
#include <Malena/Engine/Events/EventDispatcher.h>
#include <Malena/Engine/Events/EventReceiver.h>
#include <Malena/Engine/Events/Callback.h>
namespace ml
{
    class Subscribable : public EventReceiver
    {
    public:
        template<typename ENUM_TYPE>
        void subscribe(ENUM_TYPE event,
                       EventCallback);
        template<typename ENUM_TYPE>
        void subscribe(ENUM_TYPE event, Callback);

        template<typename ENUM_TYPE>
        void publish(ENUM_TYPE event, FilterCallback filter = [](EventReceiver&){return true;},
            SystemCallback resolve = nullptr,
            SystemCallback reject = nullptr);
    };

    class SubscribableDispatcher : public EventDispatcher
    {
    public:
        ///defined here since we are using a template for subscribable
        void fire(const std::optional<sf::Event> &event) override{};

        bool occurred(const std::optional<sf::Event> &event) override{};
    };
} // namespace ml
#include "../../../src/Traits/Subscribable.tpp"

ML_EXPORT(SubscribableDispatcher)
#endif // MESSAGEMANAGER_H