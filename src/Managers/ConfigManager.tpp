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
    ConfigManager<Manifest>::get(typename M::Text config) {
        ensureInitialized();
        return Manifest::template getConfig<typename M::Text, std::string>(config);
    }

    template<typename Manifest>
    template<typename M>
    std::enable_if_t<has_Ints<M>::value, int>
    ConfigManager<Manifest>::get(typename M::Integers config) {
        ensureInitialized();
        return Manifest::template getConfig<typename M::Integers, int>(config);
    }

    template<typename Manifest>
   template<typename M>
   std::enable_if_t<has_Floats<M>::value, float>
   ConfigManager<Manifest>::get(typename M::Floats config) {
        ensureInitialized();
        return Manifest::template getConfig<typename M::Floats, float>(config);
    }

    template<typename Manifest>
   template<typename M>
   std::enable_if_t<has_Booleans<M>::value, bool>
   ConfigManager<Manifest>::get(typename M::Booleans config) {
        ensureInitialized();
        return Manifest::template getConfig<typename M::Booleans, bool>(config);
    }
}

#endif //CONFIGMANAGER_TPP