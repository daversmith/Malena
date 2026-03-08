//
// Created by Dave R. Smith on 3/5/25.
//

#ifndef CUSTOMIZABLE_TPP
#define CUSTOMIZABLE_TPP
#include <Malena/Application/Application.h>
#include <Malena/Traits/Customizable.h>

namespace ml
{
    // Forwarding constructor with SFINAE to avoid ambiguity
    template<typename Component, typename Manifest>
template<typename Arg, typename... Args, typename>
Customizable<Component, Manifest>::Customizable(Arg&& arg, Args&&... args)
    : Component(std::forward<Arg>(arg), std::forward<Args>(args)...)
    {}

    template<typename Component, typename Manifest>
    template<typename T>
    void Customizable<Component, Manifest>::enableFlag(T flag) {
        this->CustomFlaggable<typename extract_Flags<Manifest>::type>::enableFlag(flag);
    }

    template<typename Component, typename Manifest>
    template<typename T>
    void Customizable<Component, Manifest>::disableFlag(T flag) {
        this->CustomFlaggable<typename extract_Flags<Manifest>::type>::disableFlag(flag);
    }

    template<typename Component, typename Manifest>
    template<typename T>
    bool Customizable<Component, Manifest>::checkFlag(T flag) const {
        return this->CustomFlaggable<typename extract_Flags<Manifest>::type>::checkFlag(flag);
    }

    // State methods - wrapper instead of using declaration
    template<typename Component, typename Manifest>
    template<typename T>
    void Customizable<Component, Manifest>::setFlag(T flag, bool value) {
        this->CustomFlaggable<typename extract_Flags<Manifest>::type>::setFlag(flag, value);
    }

    template<typename Component, typename Manifest>
    template<typename T>
    void Customizable<Component, Manifest>::toggleFlag(T flag) {
        this->CustomFlaggable<typename extract_Flags<Manifest>::type>::toggleFlag(flag);
    }
} // namespace ml
#endif