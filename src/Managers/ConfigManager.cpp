//
// Created by Dave Smith on 10/4/25.
//
#ifndef CONFIGMANAGER_CPP
#define CONFIGMANAGER_CPP
#include <Malena/Managers/ConfigManager.h>

namespace ml {
    template<typename Manifest>
    void ConfigManager<Manifest>::ensureInitialized() {
        [[maybe_unused]] static Manifest dummy;
    }

    template<typename Manifest>
    template<typename M>
    std::enable_if_t<has_Strings<M>::value, const std::string&>
    ConfigManager<Manifest>::get(typename M::Strings config) {
        ensureInitialized();
        return Manifest::template getConfig<typename M::Strings, std::string>(config);
    }

    template<typename Manifest>
    template<typename M>
    std::enable_if_t<has_Ints<M>::value, int>
    ConfigManager<Manifest>::get(typename M::Ints config) {
        ensureInitialized();
        return Manifest::template getConfig<typename M::Ints, int>(config);
    }
}

#endif //CONFIGMANAGER_TPP