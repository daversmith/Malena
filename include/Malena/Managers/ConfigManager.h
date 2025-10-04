#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <type_traits>
#include <string>
namespace ml {
    // Helper to check if Manifest has a specific type
    template<typename T, typename = void>
    struct has_Strings : std::false_type {};
    
    template<typename T>
    struct has_Strings<T, std::void_t<typename T::Strings>> : std::true_type {};
    
    template<typename T, typename = void>
    struct has_Ints : std::false_type {};
    
    template<typename T>
    struct has_Ints<T, std::void_t<typename T::Ints>> : std::true_type {};

    template<typename Manifest>
    class ConfigManager {
    public:
        static void ensureInitialized();
        
        // Only available if Manifest has Strings enum
        template<typename M = Manifest>
        static std::enable_if_t<has_Strings<M>::value, const std::string&>
        get(typename M::Strings config);
        
        // Only available if Manifest has Ints enum
        template<typename M = Manifest>
        static std::enable_if_t<has_Ints<M>::value, int>
        get(typename M::Ints config);
    };
}

#include "../../../src/Managers/ConfigManager.cpp"

#endif //CONFIGMANAGER_H